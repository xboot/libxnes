# Context Functions

The emulator context `xnes_ctx_t` is the core object of libxnes, containing all emulator components. All emulation operations are performed through the context.

## Lifecycle

### xnes_ctx_alloc

```c
struct xnes_ctx_t * xnes_ctx_alloc(const void * buf, size_t len);
```

Creates an emulator context from ROM data. `buf` is the `.nes` file data, and `len` is the data length. The function internally parses the ROM header (NES 2.0 format), loads the cartridge data, and initializes the CPU, DMA, PPU, APU, and controller.

Returns `NULL` if ROM parsing fails.

### xnes_ctx_free

```c
void xnes_ctx_free(struct xnes_ctx_t * ctx);
```

Frees the emulator context and all its resources, including the cartridge data. Passing `NULL` is safe.

### xnes_reset

```c
void xnes_reset(struct xnes_ctx_t * ctx);
```

Resets the emulator, restoring the CPU, DMA, PPU, APU, and controller to their initial state. Equivalent to pressing the NES reset button.

## Emulation Control

### xnes_step_frame

```c
uint64_t xnes_step_frame(struct xnes_ctx_t * ctx);
```

Advances emulation by one frame. Internally loops executing CPU instructions, with 3 PPU steps and 1 APU step per CPU cycle, until the PPU completes one frame (29780 CPU cycles).

**Return value**: The number of nanoseconds consumed by this frame (based on CPU frequency and speed setting), used for frame rate control.

### xnes_set_speed

```c
void xnes_set_speed(struct xnes_ctx_t * ctx, float speed);
```

Sets the emulation speed. `1.0` is normal speed, `0.5` is half speed, `2.0` is double speed. Affects the nanosecond count returned by `xnes_step_frame`.

### xnes_set_debugger

```c
void xnes_set_debugger(struct xnes_ctx_t * ctx, int (*debugger)(struct xnes_ctx_t *));
```

Sets a debugger callback function. Called before each CPU instruction executes; if the callback returns a non-zero value, emulation is paused. Pass `NULL` to disable the debugger.

```c
static int my_debugger(struct xnes_ctx_t * ctx)
{
    /* Check CPU state */
    if(ctx->cpu.pc == 0xC000)
        return 1; /* Pause at specified address */
    return 0;     /* Continue execution */
}

xnes_set_debugger(nes, my_debugger);
```

## Output

### xnes_get_pixel

```c
uint32_t xnes_get_pixel(struct xnes_ctx_t * ctx, int x, int y);
```

Gets the pixel color at the specified position in the current frame. `x` ranges from 0-255, `y` ranges from 0-239. Returns an ARGB value in `0xAARRGGBB` format (Alpha is always 0xFF).

```c
for(int y = 0; y < 240; y++)
{
    for(int x = 0; x < 256; x++)
    {
        uint32_t color = xnes_get_pixel(nes, x, y);
        /* Write to framebuffer */
    }
}
```

### xnes_set_palette

```c
void xnes_set_palette(struct xnes_ctx_t * ctx, uint32_t * pal);
```

Sets a custom palette. `pal` is an array of 64 `uint32_t` values in `0xAARRGGBB` format. Passing `NULL` has no effect. The default uses the built-in NES standard palette.

### xnes_set_audio

```c
void xnes_set_audio(struct xnes_ctx_t * ctx, void * data, void (*cb)(void *, float), int rate);
```

Sets the audio callback. The emulator calls `cb` at each audio sample; `data` is passed as the first argument to the callback, and the `float` parameter is the audio sample value (-1.0 to 1.0). `rate` is the target sample rate (e.g., 44100 or 48000).

```c
static void audio_cb(void * data, float sample)
{
    /* Write sample to audio device */
}

xnes_set_audio(nes, my_data, audio_cb, 44100);
```
