# 关于 libxnes

libxnes 是一个轻量级、可移植的纯 C99 NES 模拟器库。它实现了完整的 NES（Nintendo Entertainment System）硬件模拟，包括 CPU、PPU、APU、控制器和卡带映射器，可以将 NES 游戏渲染为像素数据并输出音频。

## 特性

- 纯 C99 实现，无外部依赖（仅 libc）；
- 完整的 6502 CPU 指令集模拟，支持 NMI / IRQ 中断；
- PPU 图形模拟：256×240 分辨率，64 色调色板，支持精灵和背景渲染；
- APU 音频模拟：2 个脉冲通道、三角波通道、噪声通道和 DMC 采样通道；
- 控制器输入：支持 2 个手柄、Zapper 光枪，支持连发（Turbo）；
- 卡带系统：支持 NES 2.0 格式，内置 15 种映射器；
- 状态保存/恢复：支持倒带（Rewind）功能；
- 可定制调色板、音频采样率、模拟速度；
- 支持调试器回调，可在每条 CPU 指令处暂停。

## 架构概览

libxnes 的核心是 `xnes_ctx_t` 上下文结构体，它持有所有模拟器组件：

```
xnes_ctx_t
├── cpu      (xnes_cpu_t)        — 6502 CPU
├── dma      (xnes_dma_t)        — OAM DMA 控制器
├── ppu      (xnes_ppu_t)        — 图像处理单元
├── apu      (xnes_apu_t)        — 音频处理单元
├── ctl      (xnes_controller_t) — 控制器输入
├── cartridge (xnes_cartridge_t *) — 卡带（含映射器）
└── palette  (uint32_t[64])      — 调色板
```

模拟流程：调用 `xnes_step_frame` 推进一帧的模拟，内部循环执行 CPU 指令，每个 CPU 周期执行 3 次 PPU 步进和 1 次 APU 步进。一帧完成后，通过 `xnes_get_pixel` 读取像素数据。

## 相关链接

- 源码仓库：[https://github.com/xboot/libxnes](https://github.com/xboot/libxnes)
- 在线文档：[https://libxnes.xboot.org](https://libxnes.xboot.org)
- Nesdev Wiki：[https://www.nesdev.org/wiki/Nesdev_Wiki](https://www.nesdev.org/wiki/Nesdev_Wiki)
- 6502 指令集：[https://www.masswerk.at/6502/6502_instruction_set.html](https://www.masswerk.at/6502/6502_instruction_set.html)
- NES 卡带数据库：[https://nescartdb.com](https://nescartdb.com)
