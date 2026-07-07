# PPU Functions

The PPU (Picture Processing Unit) is the NES's graphics processor, responsible for generating the video signal. libxnes fully emulates the PPU rendering pipeline.

## Data Structure

```c
struct xnes_ppu_t {
    struct xnes_ctx_t * ctx;
    int cycles;           // Current scanline cycle
    int scanline;         // Current scanline (0-261)
    uint64_t frame;       // Frame counter
    uint8_t front_buf[256 * 240]; // Front buffer
    uint8_t back_buf[256 * 240];  // Back buffer
    uint8_t * front;      // Current display buffer
    uint8_t * back;       // Current rendering buffer
    // ... registers and rendering state
};
```

## Rendering Parameters

- Resolution: 256×240 pixels
- 262 scanlines per frame (NTSC), of which the first 240 are visible
- 341 PPU cycles per scanline
- 1 byte index value per pixel, converted to 32-bit RGBA color via the palette
- Double buffering: the front buffer is for display, the back buffer is for rendering; they are swapped at the end of each frame

## Functions

### xnes_ppu_init

```c
void xnes_ppu_init(struct xnes_ppu_t * ppu, struct xnes_ctx_t * ctx);
```

Initializes the PPU. Zeroes all buffers and registers, and sets the front/back buffer pointers. Usually called internally by `xnes_ctx_alloc`.

### xnes_ppu_reset

```c
void xnes_ppu_reset(struct xnes_ppu_t * ppu);
```

Resets the PPU. Resets all registers, scanline, and frame counters.

### xnes_ppu_step

```c
void xnes_ppu_step(struct xnes_ppu_t * ppu);
```

Advances one PPU cycle. Internally handles pixel rendering, sprite evaluation, NMI signaling, etc. Called 3 times per CPU cycle. Usually called internally by `xnes_step_frame`.

### xnes_ppu_read_register

```c
uint8_t xnes_ppu_read_register(struct xnes_ppu_t * ppu, uint16_t addr);
```

Reads a PPU register. Address range $2000-$2007, mirrored every 8 bytes.

Main registers:

| Address | Name | Description |
|---------|------|-------------|
| $2002 | PPUSTATUS | PPU status (V-blank, Sprite 0 hit, Overflow) |
| $2004 | OAMDATA | OAM data |
| $2007 | PPUDATA | PPU data |

### xnes_ppu_write_register

```c
void xnes_ppu_write_register(struct xnes_ppu_t * ppu, uint16_t addr, uint8_t val);
```

Writes a PPU register.

Main registers:

| Address | Name | Description |
|---------|------|-------------|
| $2000 | PPUCTRL | Control (NMI, background/sprite address, address increment) |
| $2001 | PPUMASK | Rendering control (display switch, grayscale, masking) |
| $2003 | OAMADDR | OAM address |
| $2005 | PPUSCROLL | Scroll offset |
| $2006 | PPUADDR | PPU address |
| $2007 | PPUDATA | PPU data |

### xnes_ppu_is_white_pixel

```c
uint8_t xnes_ppu_is_white_pixel(struct xnes_ppu_t * ppu, int x, int y);
```

Checks whether the specified pixel is non-black (used for Zapper light gun hit detection). A non-zero return value indicates that the pixel is bright.

## Pixel Reading

Read pixel colors via the context function `xnes_get_pixel`:

```c
uint32_t color = xnes_get_pixel(ctx, x, y);
```

This function reads the pixel index from the PPU front buffer and converts it to an RGBA color via the context palette.
