# Controller Functions

libxnes supports standard NES controller input, including 2 gamepads, the Zapper light gun, and turbo functionality.

## Gamepad Buttons

```c
enum {
    XNES_JOYSTICK_A      = (1 << 7), // A button
    XNES_JOYSTICK_B      = (1 << 6), // B button
    XNES_JOYSTICK_SELECT = (1 << 5), // Select button
    XNES_JOYSTICK_START  = (1 << 4), // Start button
    XNES_JOYSTICK_UP     = (1 << 3), // Up
    XNES_JOYSTICK_DOWN   = (1 << 2), // Down
    XNES_JOYSTICK_LEFT   = (1 << 1), // Left
    XNES_JOYSTICK_RIGHT  = (1 << 0), // Right
};
```

## Turbo Speeds

```c
enum {
    XNES_CONTROLLER_TURBO_SPEED_FAST   = (1 << 1), // Fast turbo
    XNES_CONTROLLER_TURBO_SPEED_NORMAL = (1 << 2), // Normal turbo
    XNES_CONTROLLER_TURBO_SPEED_SLOW   = (1 << 3), // Slow turbo
};
```

## Data Structure

```c
struct xnes_controller_t {
    struct xnes_ctx_t * ctx;
    uint8_t turbo_count;  // Turbo counter
    uint8_t turbo_speed;  // Turbo speed
    uint8_t latch;        // Shift register latch
    struct {
        struct { uint8_t key, key_turbo, key_index; } p1;
        struct { uint8_t key, key_turbo, key_index; } p2;
    } joystick;
    struct { uint8_t x, y, trigger; } zapper; // Zapper light gun
};
```

## Functions

### xnes_controller_init

```c
void xnes_controller_init(struct xnes_controller_t * ctl, struct xnes_ctx_t * ctx);
```

Initializes the controller. Zeroes all state and sets the default turbo speed. Usually called internally by `xnes_ctx_alloc`.

### xnes_controller_reset

```c
void xnes_controller_reset(struct xnes_controller_t * ctl);
```

Resets the controller, zeroing all button states.

### xnes_controller_set_turbo_speed

```c
void xnes_controller_set_turbo_speed(struct xnes_controller_t * ctl, uint8_t speed);
```

Sets the turbo speed. Use a combination of `XNES_CONTROLLER_TURBO_SPEED_*` macros.

### xnes_controller_joystick_p1 / p2

```c
void xnes_controller_joystick_p1(struct xnes_controller_t * ctl, uint8_t down, uint8_t up);
void xnes_controller_joystick_p2(struct xnes_controller_t * ctl, uint8_t down, uint8_t up);
```

Sets the gamepad button state for player 1 or player 2. `down` is the mask of buttons pressed in this update, and `up` is the mask of buttons released. Use a combination of `XNES_JOYSTICK_*` macros.

```c
/* Player 1 presses the A button and right directional button */
xnes_controller_joystick_p1(&ctx->ctl, XNES_JOYSTICK_A | XNES_JOYSTICK_RIGHT, 0);

/* Player 1 releases the A button */
xnes_controller_joystick_p1(&ctx->ctl, 0, XNES_JOYSTICK_A);
```

### xnes_controller_joystick_p1_turbo / p2_turbo

```c
void xnes_controller_joystick_p1_turbo(struct xnes_controller_t * ctl, uint8_t down, uint8_t up);
void xnes_controller_joystick_p2_turbo(struct xnes_controller_t * ctl, uint8_t down, uint8_t up);
```

Sets the turbo button state. When held, the button automatically repeats press/release at the turbo speed. Parameters are the same as above.

### xnes_controller_zapper

```c
void xnes_controller_zapper(struct xnes_controller_t * ctl, uint8_t x, uint8_t y, uint8_t trigger);
```

Sets the Zapper light gun state. `x` and `y` are screen coordinates (0-255, 0-239), and a non-zero `trigger` indicates the trigger is pulled. The emulator internally determines whether a hit occurred by checking the pixel brightness at the corresponding PPU position.

```c
/* Pull the trigger */
xnes_controller_zapper(&ctx->ctl, 128, 120, 1);

/* Release the trigger */
xnes_controller_zapper(&ctx->ctl, 128, 120, 0);
```

### xnes_controller_read_register / write_register

```c
uint8_t xnes_controller_read_register(struct xnes_controller_t * ctl, uint16_t addr);
void xnes_controller_write_register(struct xnes_controller_t * ctl, uint16_t addr, uint8_t val);
```

Reads and writes controller registers ($4016/$4017). Called internally by the CPU; usually no need to use directly.
