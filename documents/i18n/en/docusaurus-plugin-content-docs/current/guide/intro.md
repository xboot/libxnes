# About libxnes

libxnes is a lightweight, portable pure C99 NES emulator library. It implements complete NES (Nintendo Entertainment System) hardware emulation, including CPU, PPU, APU, controllers and cartridge mappers, and can render NES games to pixel data and output audio.

## Features

- Pure C99 implementation, no external dependencies (only libc);
- Complete 6502 CPU instruction set emulation, with NMI / IRQ interrupt support;
- PPU graphics emulation: 256×240 resolution, 64-color palette, sprite and background rendering support;
- APU audio emulation: 2 pulse channels, a triangle channel, a noise channel and a DMC sample channel;
- Controller input: 2 gamepads and Zapper light gun supported, with Turbo (auto-fire) support;
- Cartridge system: NES 2.0 format supported, 15 built-in mappers;
- State save/restore: Rewind functionality supported;
- Customizable palette, audio sample rate and emulation speed;
- Debugger callback support, allowing a pause at every CPU instruction.

## Architecture Overview

The core of libxnes is the `xnes_ctx_t` context structure, which holds all emulator components:

```
xnes_ctx_t
├── cpu      (xnes_cpu_t)        — 6502 CPU
├── dma      (xnes_dma_t)        — OAM DMA controller
├── ppu      (xnes_ppu_t)        — Picture Processing Unit
├── apu      (xnes_apu_t)        — Audio Processing Unit
├── ctl      (xnes_controller_t) — Controller input
├── cartridge (xnes_cartridge_t *) — Cartridge (including mapper)
└── palette  (uint32_t[64])      — Palette
```

Emulation flow: call `xnes_step_frame` to advance the emulation by one frame. Internally it loops executing CPU instructions, performing 3 PPU steps and 1 APU step for each CPU cycle. Once a frame is complete, pixel data can be read via `xnes_get_pixel`.

## Related Links

- Source repository: [https://github.com/xboot/libxnes](https://github.com/xboot/libxnes)
- Online documentation: [https://libxnes.xboot.org](https://libxnes.xboot.org)
- Nesdev Wiki: [https://www.nesdev.org/wiki/Nesdev_Wiki](https://www.nesdev.org/wiki/Nesdev_Wiki)
- 6502 instruction set: [https://www.masswerk.at/6502/6502_instruction_set.html](https://www.masswerk.at/6502/6502_instruction_set.html)
- NES cartridge database: [https://nescartdb.com](https://nescartdb.com)
