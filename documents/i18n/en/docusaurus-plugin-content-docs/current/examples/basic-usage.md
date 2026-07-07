# Basic Usage

This example demonstrates how to use libxnes to load a ROM, run the emulation, and display the picture. Based on the Linux SDL2 example.

## Screenshots

![super-mario](/screenshots/super-mario.png)
![contra](/screenshots/contra.png)
![battle-city](/screenshots/battle-city.png)

## Complete Workflow

### 1. Load the ROM file

```c
#include <xnes.h>

static void * file_load(const char * filename, uint64_t * len)
{
    FILE * in = fopen(filename, "rb");
    if(in)
    {
        uint64_t offset = 0, bufsize = 8192;
        char * buf = malloc(bufsize);
        while(1)
        {
            uint64_t n = fread(buf + offset, 1, bufsize - offset, in);
            offset += n;
            if(n < bufsize - offset)
                break;
            bufsize *= 2;
            buf = realloc(buf, bufsize);
        }
        if(len)
            *len = offset;
        fclose(in);
        return buf;
    }
    return NULL;
}

void * buf;
uint64_t len;
buf = file_load("game.nes", &len);
struct xnes_ctx_t * nes = xnes_ctx_alloc(buf, len);
free(buf);
```

### 2. Create a state manager (used for rewinding)

```c
struct xnes_state_t * state = xnes_state_alloc(nes, 60 * 30);
```

### 3. Frame loop

```c
uint64_t timestamp = ktime_get();
uint64_t elapsed = 0;

while(running)
{
    if(ktime_get() - timestamp >= elapsed)
    {
        timestamp = ktime_get();
        elapsed = xnes_step_frame(nes);  // Emulate one frame
        screen_refresh(nes);              // Display the picture
        xnes_state_push(state);           // Save the state
    }
    else
        SDL_Delay(1);
}
```

### 4. Read and display pixels

```c
void screen_refresh(struct xnes_ctx_t * nes)
{
    uint32_t * fb = /* your framebuffer */;
    for(int y = 0; y < 240; y++)
    {
        for(int x = 0; x < 256; x++)
        {
            fb[y * 256 + x] = xnes_get_pixel(nes, x, y);
        }
    }
}
```

### 5. Cleanup

```c
xnes_state_free(state);
xnes_ctx_free(nes);
```

## More Screenshots

![flying-hero](/screenshots/flying-hero.png)
![jackal](/screenshots/jackal.png)
![excite-bike](/screenshots/excite-bike.png)
![bomber-man](/screenshots/bomber-man.png)
![duck-hunt](/screenshots/duck-hunt.png)

:::note
See the repository `examples/linux/main.c` for the complete example code.
:::
