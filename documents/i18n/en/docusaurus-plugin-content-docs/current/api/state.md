# State Functions

libxnes provides state save/restore functionality, which can be used to implement rewind and save states. The state data contains a complete snapshot of the CPU, DMA, PPU, APU, and cartridge.

## Data Structure

```c
struct xnes_state_t {
    struct xnes_ctx_t * ctx;  // Associated emulator context
    unsigned char * buffer;   // State ring buffer
    uint32_t length;          // Size of a single state
    uint32_t count;           // Buffer capacity (number of states)
    uint32_t in;              // Write position
    uint32_t out;             // Read position
};
```

The state buffer is a ring queue, with `in` and `out` as the write and read indices. When the queue is full, the oldest state is automatically discarded.

## Functions

### xnes_state_alloc

```c
struct xnes_state_t * xnes_state_alloc(struct xnes_ctx_t * ctx, int count);
```

Creates a state manager. `count` specifies the buffer capacity (the number of states that can be saved). For example, `count = 60 * 30` means saving the last 30 seconds of states (60 frames/second).

```c
/* Save the last 30 seconds of state (for rewind) */
struct xnes_state_t * state = xnes_state_alloc(nes, 60 * 30);
```

### xnes_state_free

```c
void xnes_state_free(struct xnes_state_t * state);
```

Frees the state manager and its buffer. Passing `NULL` is safe.

### xnes_state_push

```c
void xnes_state_push(struct xnes_state_t * state);
```

Saves the current emulator state (pushes onto the stack). Usually called after each frame of emulation is complete. If the buffer is full, the oldest state is discarded.

```c
xnes_step_frame(nes);    // Emulate one frame
xnes_state_push(state);  // Save state
```

### xnes_state_pop

```c
void xnes_state_pop(struct xnes_state_t * state);
```

Restores the last saved state (pops from the stack). After restoration, the emulator state returns to the exact state at the time it was saved.

```c
xnes_state_pop(state);   // Go back one frame
```

## Rewind Implementation

Implement the rewind feature through continuous push and pop operations:

```c
/* Forward game loop */
while(!rewind)
{
    uint64_t elapsed = xnes_step_frame(nes);
    xnes_state_push(state);
    /* Wait for elapsed nanoseconds */
}

/* Rewind loop */
while(rewind)
{
    xnes_state_pop(state);
    /* Display current frame */
    /* Control rewind speed */
}
```

:::note
When restoring state, the controller is automatically reset to avoid leftover input state.
:::

## State Contents

Each state snapshot contains the following data:

| Component | Contents |
|-----------|----------|
| CPU | Registers, RAM, PC, SP, cycle count |
| DMA | DMA state |
| PPU | Registers, nametables, OAM, palette, rendering state |
| APU | All channel states, frame counter |
| Cartridge | Mapper state, SRAM, PRG-RAM, CHR-RAM |

The state size depends on the cartridge's memory configuration (PRG-RAM, CHR-RAM size), typically ranging from a few KB to tens of KB.
