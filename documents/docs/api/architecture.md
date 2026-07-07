# 架构概述

libxnes 是一个完整的 NES 模拟器库，采用组件化设计。核心是 `xnes_ctx_t` 上下文结构体，它持有所有模拟器组件。

## 上下文结构

```c
struct xnes_ctx_t {
    struct xnes_cpu_t cpu;            // 6502 CPU
    struct xnes_dma_t dma;            // OAM DMA 控制器
    struct xnes_ppu_t ppu;            // 图像处理单元
    struct xnes_apu_t apu;            // 音频处理单元
    struct xnes_controller_t ctl;     // 控制器输入
    struct xnes_cartridge_t * cartridge; // 卡带（含映射器）
    uint32_t palette[64];             // 调色板
};
```

## 模拟流程

libxnes 的核心模拟循环是 `xnes_step_frame`，它推进一帧的模拟：

```
xnes_step_frame(ctx)
    └── 循环执行 xnes_step(ctx) 直到 PPU 帧计数器递增
        └── xnes_cpu_step(&ctx->cpu)  — 执行一条 CPU 指令，返回周期数
            └── 每个周期执行 3 次 xnes_ppu_step(&ctx->ppu)
            └── 每个周期执行 1 次 xnes_apu_step(&ctx->apu)
```

一个 NTSC NES 帧包含 29780 个 CPU 周期（约 1/60 秒）。

## 组件说明

| 组件 | 结构体 | 说明 |
|------|--------|------|
| CPU | `xnes_cpu_t` | 6502 处理器，2KB 内部 RAM，支持 NMI/IRQ 中断 |
| PPU | `xnes_ppu_t` | 图像处理单元，256×240 分辨率，双缓冲 |
| APU | `xnes_apu_t` | 音频处理单元，5 个声道 + 低通滤波 |
| DMA | `xnes_dma_t` | OAM DMA，用于快速传输精灵数据 |
| 控制器 | `xnes_controller_t` | 2 个手柄 + Zapper 光枪，支持连发 |
| 卡带 | `xnes_cartridge_t` | ROM 加载，映射器模拟，SRAM 存储 |

## 内存映射

NES 的 CPU 地址空间：

| 地址范围 | 说明 |
|----------|------|
| $0000-$07FF | 2KB 内部 RAM |
| $0800-$1FFF | RAM 镜像 |
| $2000-$2007 | PPU 寄存器 |
| $2008-$3FFF | PPU 寄存器镜像 |
| $4000-$4017 | APU 和控制器寄存器 |
| $4018-$401F | 功能测试 |
| $4020-$FFFF | 卡带空间（PRG-ROM / SRAM） |

## 配置

libxnes 通过 `xnesconf.h` 头文件提供可覆盖的配置宏：

```c
#ifndef xnes_malloc
#define xnes_malloc     malloc
#endif

#ifndef xnes_free
#define xnes_free       free
#endif

#ifndef xnes_memcpy
#define xnes_memcpy     memcpy
#endif

#ifndef xnes_memset
#define xnes_memset     memset
#endif
```

在包含 `xnes.h` 之前定义这些宏，可以替换内存分配函数，实现自定义内存管理。

## 接下来

- [Context 函数](./context)：模拟器上下文的生命周期与控制
- [Cartridge 函数](./cartridge)：卡带加载与映射器系统
- [CPU 函数](./cpu)：6502 CPU 的操作
- [PPU 函数](./ppu)：图像处理单元
- [APU 函数](./apu)：音频处理单元
- [Controller 函数](./controller)：控制器输入
- [State 函数](./state)：状态保存与倒带
