# Cartridge 函数

卡带系统负责加载 NES ROM 数据，并通过映射器（Mapper）实现地址译码和 bank 切换。libxnes 支持 NES 2.0 格式的 ROM，内置 15 种映射器。

## 数据结构

```c
struct xnes_cartridge_t {
    struct { /* NES 2.0 头部 */ } header;
    uint16_t mapper_number;    // 映射器编号
    uint8_t mirror;            // 镜像模式
    uint8_t timing;            // 时序类型 (NTSC/PAL/Dendy)
    uint8_t * prg_rom;         // PRG-ROM 数据
    uint8_t * prg_ram;         // PRG-RAM (电池备份)
    uint8_t * chr_rom;         // CHR-ROM / CHR-RAM 数据
    // ...
    struct { /* 映射器状态与回调 */ } mapper;
};
```

## 镜像模式

```c
enum {
    XNES_CARTRIDGE_MIRROR_HORIZONTAL  = 0, // 水平镜像
    XNES_CARTRIDGE_MIRROR_VERTICAL    = 1, // 垂直镜像
    XNES_CARTRIDGE_MIRROR_FOUR_SCREEN = 2, // 四屏
    XNES_CARTRIDGE_MIRROR_SINGLE0     = 3, // 单屏（命名表 0）
    XNES_CARTRIDGE_MIRROR_SINGLE1     = 4, // 单屏（命名表 1）
};
```

## 时序类型

```c
enum {
    XNES_CARTRIDGE_TIMING_NTSC   = 0, // NTSC (60 Hz, 29780 cycles/frame)
    XNES_CARTRIDGE_TIMING_PAL    = 1, // PAL (50 Hz, 33260 cycles/frame)
    XNES_CARTRIDGE_TIMING_DENDY  = 2, // Dendy (50 Hz, 35464 cycles/frame)
};
```

## 函数

### xnes_cartridge_alloc

```c
struct xnes_cartridge_t * xnes_cartridge_alloc(const void * buf, size_t len, struct xnes_ctx_t * ctx);
```

从 ROM 数据创建卡带。解析 NES 2.0 头部，分配 PRG-ROM、CHR-ROM、PRG-RAM 等内存，并根据映射器编号初始化对应的映射器。返回 `NULL` 表示解析失败。

通常由 `xnes_ctx_alloc` 内部调用，无需直接使用。

### xnes_cartridge_free

```c
void xnes_cartridge_free(struct xnes_cartridge_t * c);
```

释放卡带及其所有分配的内存。

### 映射器接口

映射器通过函数指针提供 CPU/PPU 地址空间的读写回调：

```c
uint8_t xnes_cartridge_mapper_cpu_read(struct xnes_ctx_t * ctx, uint16_t addr);
void xnes_cartridge_mapper_cpu_write(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val);
uint8_t xnes_cartridge_mapper_ppu_read(struct xnes_ctx_t * ctx, uint16_t addr);
void xnes_cartridge_mapper_ppu_write(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val);
void xnes_cartridge_mapper_apu_step(struct xnes_ctx_t * ctx);
void xnes_cartridge_mapper_ppu_step(struct xnes_ctx_t * ctx);
```

这些函数由 CPU、PPU 和 APU 在模拟循环中调用，映射器根据自身逻辑进行 bank 切换和地址译码。部分映射器（如 Mapper 4）会在 APU 或 PPU 步进时检查 IRQ 条件。

## 支持的映射器

完整列表参见[映射器参考列表](../reference/mappers)。
