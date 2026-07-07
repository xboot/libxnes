# Input Handling

This example demonstrates how to handle keyboard and gamepad input, converting it into libxnes controller state.

## Keyboard input

Map keyboard events to NES gamepad buttons:

```c
/* Key pressed */
case SDL_KEYDOWN:
    switch(e.key.keysym.sym)
    {
    case SDLK_w:
        xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_UP, 0);
        break;
    case SDLK_s:
        xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_DOWN, 0);
        break;
    case SDLK_a:
        xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_LEFT, 0);
        break;
    case SDLK_d:
        xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_RIGHT, 0);
        break;
    case SDLK_k:
        xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_A, 0);
        break;
    case SDLK_j:
        xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_B, 0);
        break;
    case SDLK_SPACE:
        xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_SELECT, 0);
        break;
    case SDLK_RETURN:
        xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_START, 0);
        break;
    case SDLK_ESCAPE:
        xnes_reset(nes);
        break;
    }
    break;

/* Key released */
case SDL_KEYUP:
    switch(e.key.keysym.sym)
    {
    case SDLK_w:
        xnes_controller_joystick_p1(&nes->ctl, 0, XNES_JOYSTICK_UP);
        break;
    /* ... other keys are similar ... */
    }
    break;
```

## Gamepad input

Map SDL gamepad events to the NES controller:

### Stick axes

```c
case SDL_JOYAXISMOTION:
    if(e.jaxis.axis == 0)  /* X axis */
    {
        if(e.jaxis.value < -3200)
            xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_LEFT, 0);
        else if(e.jaxis.value > 3200)
            xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_RIGHT, 0);
        else
            xnes_controller_joystick_p1(&nes->ctl, 0, XNES_JOYSTICK_LEFT | XNES_JOYSTICK_RIGHT);
    }
    else if(e.jaxis.axis == 1)  /* Y axis */
    {
        if(e.jaxis.value < -3200)
            xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_UP, 0);
        else if(e.jaxis.value > 3200)
            xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_DOWN, 0);
        else
            xnes_controller_joystick_p1(&nes->ctl, 0, XNES_JOYSTICK_UP | XNES_JOYSTICK_DOWN);
    }
    break;
```

### Gamepad buttons

```c
case SDL_JOYBUTTONDOWN:
    switch(e.jbutton.button)
    {
    case 0: /* B */
        xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_B, 0);
        break;
    case 1: /* A */
        xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_A, 0);
        break;
    case 2: /* Y - turbo B */
        xnes_controller_joystick_p1_turbo(&nes->ctl, XNES_JOYSTICK_B, 0);
        break;
    case 3: /* X - turbo A */
        xnes_controller_joystick_p1_turbo(&nes->ctl, XNES_JOYSTICK_A, 0);
        break;
    case 6: /* Select */
        xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_SELECT, 0);
        break;
    case 7: /* Start */
        xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_START, 0);
        break;
    }
    break;
```

## Speed control

Control the emulation speed via `xnes_set_speed`:

```c
case SDLK_F1:
    xnes_set_speed(nes, 0.5);  /* Slow speed */
    break;
case SDLK_F2:
    xnes_set_speed(nes, 2.0);  /* Fast speed */
    break;
/* Restore on release */
case SDL_KEYUP:
    if(e.key.keysym.sym == SDLK_F1 || e.key.keysym.sym == SDLK_F2)
        xnes_set_speed(nes, 1.0);  /* Normal speed */
    break;
```

## Zapper light gun

For games that support the light gun (such as Duck Hunt), use the mouse as the zapper:

```c
case SDL_MOUSEBUTTONDOWN:
    if(e.button.button == SDL_BUTTON_LEFT)
        xnes_controller_zapper(&nes->ctl,
            e.button.x / SCALE, e.button.y / SCALE, 1);
    break;

case SDL_MOUSEBUTTONUP:
    if(e.button.button == SDL_BUTTON_LEFT)
        xnes_controller_zapper(&nes->ctl,
            e.button.x / SCALE, e.button.y / SCALE, 0);
    break;
```

See [Controls Reference](../reference/controls) for the complete key mapping.
