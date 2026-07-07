# Getting Started

This section walks you through a simple example to help you get started with libxnes in minutes.

## Prerequisites

1. You have built libxnes following [Build and Install](./build-guide);
2. You have a `.nes` format ROM file ready.

## Step 1: Load the ROM

Read the ROM file data and create the emulator context:

```c
#include <xnes.h>

/* Read the ROM file into memory */
FILE * f = fopen("game.nes", "rb");
fseek(f, 0, SEEK_END);
size_t len = ftell(f);
fseek(f, 0, SEEK_SET);
void * buf = malloc(len);
fread(buf, 1, len, f);
fclose(f);

/* Create the emulator context */
struct xnes_ctx_t * nes = xnes_ctx_alloc(buf, len);
free(buf);
```

## Step 2: Run a Frame

Call `xnes_step_frame` to advance the emulation by one frame. It returns the number of nanoseconds consumed by that frame:

```c
uint64_t elapsed = xnes_step_frame(nes);
```

## Step 3: Read Pixels

Read the 256×240 resolution pixel data via `xnes_get_pixel`:

```c
for(int y = 0; y < 240; y++)
{
    for(int x = 0; x < 256; x++)
    {
        uint32_t color = xnes_get_pixel(nes, x, y);
        /* color is an RGBA value in 0xAARRGGBB format */
    }
}
```

## Step 4: Set Up Audio

Register an audio callback function to receive the audio samples output by the emulator:

```c
static void audio_callback(void * data, float sample)
{
    /* Write sample to the audio device */
}

xnes_set_audio(nes, NULL, audio_callback, 44100);
```

## Step 5: Handle Input

Set gamepad button states via the controller functions:

```c
/* Player 1 presses the A button */
xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_A, 0);

/* Player 1 releases the A button */
xnes_controller_joystick_p1(&nes->ctl, 0, XNES_JOYSTICK_A);
```

## Step 6: Destroy Resources

```c
xnes_ctx_free(nes);
```

## Complete Example

See the [Linux SDL2 example](../examples/basic-usage), which demonstrates complete window creation, frame loop, audio output and input handling.

## More Operations

- [API Reference](../api/architecture): Learn the detailed descriptions of all available functions;
- [Examples](../examples/basic-usage): View complete code for various features;
- [Reference List](../reference/mappers): View the list of supported mappers and key mappings.
