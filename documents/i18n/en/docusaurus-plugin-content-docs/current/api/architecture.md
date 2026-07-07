# Architecture Overview

libxnes is a complete NES emulator library with a component-based design. The core is the `xnes_ctx_t` context struct, which holds all emulator components.

## Context Structure

```c
struct xnes_ctx_t {
    struct xnes_cpu_t cpu;            // 6502 CPU
    struct xnes_dma_t dma;            // OAM DMA controller
    struct xnes_ppu_t ppu;            // Picture Processing Unit
    struct xnes_apu_t apu;            // Audio Processing Unit
    struct xnes_controller_t ctl;     // Controller input
    struct xnes_cartridge_t * cartridge; // Cartridge (including mapper)
    uint32_t palette[64];             // Palette
};
```

## Emulation Flow

The core emulation loop of libxnes is `xnes_step_frame`, which advances emulation by one frame:

```
xnes_step_frame(ctx)
    └── Loop executing xnes_step(ctx) until the PPU frame counter increments
        └── xnes_cpu_step(&ctx->cpu)  — Execute one CPU instruction, returns cycle count
            └── Execute xnes_ppu_step(&ctx->ppu) 3 times per cycle
            └── Execute xnes_apu_step(&ctx->apu) 1 time per cycle
```

One NTSC NES frame contains 29780 CPU cycles (approximately 1/60 second).

## Component Description

| Component | Struct | Description |
|-----------|--------|-------------|
| CPU | `xnes_cpu_t` | 6502 processor, 2KB internal RAM, supports NMI/IRQ interrupts |
| PPU | `xnes_ppu_t` | Picture Processing Unit, 256×240 resolution, double buffering |
| APU | `xnes_apu_t` | Audio Processing Unit, 5 channels + low-pass filtering |
| DMA | `xnes_dma_t` | OAM DMA, used for fast sprite data transfer |
| Controller | `xnes_controller_t` | 2 gamepads + Zapper light gun, supports turbo |
| Cartridge | `xnes_cartridge_t` | ROM loading, mapper emulation, SRAM storage |

## Memory Map

NES CPU address space:

| Address Range | Description |
|---------------|-------------|
| $0000-$07FF | 2KB internal RAM |
| $0800-$1FFF | RAM mirrors |
| $2000-$2007 | PPU registers |
| $2008-$3FFF | PPU register mirrors |
| $4000-$4017 | APU and controller registers |
| $4018-$401F | Function testing |
| $4020-$FFFF | Cartridge space (PRG-ROM / SRAM) |

## Configuration

libxnes provides overridable configuration macros via the `xnesconf.h` header file:

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

Define these macros before including `xnes.h` to replace the memory allocation functions and implement custom memory management.

## Next

- [Context Functions](./context): Lifecycle and control of the emulator context
- [Cartridge Functions](./cartridge): Cartridge loading and mapper system
- [CPU Functions](./cpu): 6502 CPU operations
- [PPU Functions](./ppu): Picture Processing Unit
- [APU Functions](./apu): Audio Processing Unit
- [Controller Functions](./controller): Controller input
- [State Functions](./state): State saving and rewind
