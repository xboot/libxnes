# PPU 函数

PPU（Picture Processing Unit）是 NES 的图形处理器，负责生成视频信号。libxnes 完整模拟了 PPU 的渲染管线。

## 数据结构

```c
struct xnes_ppu_t {
    struct xnes_ctx_t * ctx;
    int cycles;           // 当前扫描线周期
    int scanline;         // 当前扫描线 (0-261)
    uint64_t frame;       // 帧计数器
    uint8_t front_buf[256 * 240]; // 前台缓冲
    uint8_t back_buf[256 * 240];  // 后台缓冲
    uint8_t * front;      // 当前显示缓冲
    uint8_t * back;       // 当前渲染缓冲
    // ... 寄存器和渲染状态
};
```

## 渲染参数

- 分辨率：256×240 像素
- 每帧 262 条扫描线（NTSC），其中前 240 条为可见行
- 每条扫描线 341 个 PPU 周期
- 每个像素 1 字节索引值，通过调色板转换为 32 位 RGBA 颜色
- 双缓冲：前台缓冲用于显示，后台缓冲用于渲染，帧结束时交换

## 函数

### xnes_ppu_init

```c
void xnes_ppu_init(struct xnes_ppu_t * ppu, struct xnes_ctx_t * ctx);
```

初始化 PPU。清零所有缓冲区和寄存器，设置前台/后台缓冲指针。通常由 `xnes_ctx_alloc` 内部调用。

### xnes_ppu_reset

```c
void xnes_ppu_reset(struct xnes_ppu_t * ppu);
```

复位 PPU。重置所有寄存器、扫描线和帧计数器。

### xnes_ppu_step

```c
void xnes_ppu_step(struct xnes_ppu_t * ppu);
```

推进一个 PPU 周期。内部处理像素渲染、精灵评估、NMI 信号等。每个 CPU 周期调用 3 次。通常由 `xnes_step_frame` 内部调用。

### xnes_ppu_read_register

```c
uint8_t xnes_ppu_read_register(struct xnes_ppu_t * ppu, uint16_t addr);
```

读取 PPU 寄存器。地址范围 $2000-$2007，每 8 字节镜像。

主要寄存器：

| 地址 | 名称 | 说明 |
|------|------|------|
| $2002 | PPUSTATUS | PPU 状态（V-blank、Sprite 0 hit、Overflow） |
| $2004 | OAMDATA | OAM 数据 |
| $2007 | PPUDATA | PPU 数据 |

### xnes_ppu_write_register

```c
void xnes_ppu_write_register(struct xnes_ppu_t * ppu, uint16_t addr, uint8_t val);
```

写入 PPU 寄存器。

主要寄存器：

| 地址 | 名称 | 说明 |
|------|------|------|
| $2000 | PPUCTRL | 控制（NMI、背景/精灵地址、地址递增） |
| $2001 | PPUMASK | 渲染控制（显示开关、灰度、遮罩） |
| $2003 | OAMADDR | OAM 地址 |
| $2005 | PPUSCROLL | 滚动偏移 |
| $2006 | PPUADDR | PPU 地址 |
| $2007 | PPUDATA | PPU 数据 |

### xnes_ppu_is_white_pixel

```c
uint8_t xnes_ppu_is_white_pixel(struct xnes_ppu_t * ppu, int x, int y);
```

检查指定像素是否为非黑色（用于 Zapper 光枪的命中检测）。返回非零表示该像素为亮色。

## 像素读取

通过上下文函数 `xnes_get_pixel` 读取像素颜色：

```c
uint32_t color = xnes_get_pixel(ctx, x, y);
```

该函数从 PPU 前台缓冲读取像素索引，通过上下文调色板转换为 RGBA 颜色。
