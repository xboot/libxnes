# Rewind

This example demonstrates how to use libxnes's state save/restore functionality to implement rewind.

## Principle

Rewind is implemented by continuously saving emulator state snapshots. While playing, the state is saved every frame; while rewinding, previous states are restored frame by frame.

```
Forward play:  step_frame → push → step_frame → push → step_frame → push ...
Rewind:        pop → display → pop → display → pop → display ...
```

## Implementation

### 1. Create a state manager

```c
struct xnes_ctx_t * nes = xnes_ctx_alloc(buf, len);
/* Save the last 30 seconds of state (60 fps × 30 seconds = 1800 frames) */
struct xnes_state_t * state = xnes_state_alloc(nes, 60 * 30);
int rewind = 0;
```

### 2. Main loop

```c
uint64_t timestamp = ktime_get();
uint64_t elapsed = 0;

while(running)
{
    /* Handle input */
    handle_events(nes, &rewind);

    if(rewind)
    {
        /* Rewind mode: restore state at a fixed interval */
        if(ktime_get() - timestamp >= 16666666)  /* about 60 FPS */
        {
            timestamp = ktime_get();
            elapsed = 16666666;
            xnes_state_pop(state);        /* Restore the previous frame */
            screen_refresh(nes);           /* Display the picture */
        }
        else
            SDL_Delay(1);
    }
    else
    {
        /* Normal mode: emulate one frame */
        if(ktime_get() - timestamp >= elapsed)
        {
            timestamp = ktime_get();
            elapsed = xnes_step_frame(nes);  /* Emulate one frame */
            screen_refresh(nes);              /* Display the picture */
            xnes_state_push(state);           /* Save the state */
        }
        else
            SDL_Delay(1);
    }
}
```

### 3. Trigger rewind

Trigger via the keyboard R key or gamepad L key:

```c
case SDL_KEYDOWN:
    if(e.key.keysym.sym == SDLK_r)
        rewind = 1;
    break;

case SDL_KEYUP:
    if(e.key.keysym.sym == SDLK_r)
        rewind = 0;
    break;
```

### 4. Cleanup

```c
xnes_state_free(state);
xnes_ctx_free(nes);
```

## Screenshots

![1943](/screenshots/1943.png)
![circus-chablic](/screenshots/circus-chablic.png)
![lode-runner](/screenshots/lode-runner.png)

:::note
- The state buffer is a circular queue; when the queue is full, the oldest state is automatically discarded.
- When a state is restored, the controller is automatically reset to avoid leftover input state.
- The state size depends on the cartridge's memory configuration, typically ranging from a few KB to tens of KB.
:::
