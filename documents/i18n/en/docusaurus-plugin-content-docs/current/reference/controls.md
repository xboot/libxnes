# Controls Reference

## Keyboard Mapping

| Function | Player 1 | Player 2 |
|------|--------|--------|
| Up (UP) | W | Arrow Up |
| Down (DOWN) | S | Arrow Down |
| Left (LEFT) | A | Arrow Left |
| Right (RIGHT) | D | Arrow Right |
| A Button | K | Numpad 6 |
| B Button | J | Numpad 5 |
| SELECT | Space | Numpad 8 |
| START | Enter | Numpad 9 |

| Function | Key |
|------|------|
| RESET | ESC |
| Slow Speed (SLOW SPEED) | F1 |
| Fast Speed (FAST SPEED) | F2 |
| Rewind (REWIND) | R |

## Gamepad Mapping

| Function | Gamepad 1 | Gamepad 2 |
|------|--------|--------|
| Up (UP) | Up | Up |
| Down (DOWN) | Down | Down |
| Left (LEFT) | Left | Left |
| Right (RIGHT) | Right | Right |
| A Button | A | A |
| B Button | B | B |
| SELECT | Select | Select |
| START | Start | Start |
| Rewind (REWIND) | L | L |
| Fast Speed (FAST SPEED) | R | R |

## Gamepad Turbo

| Function | Gamepad 1 | Gamepad 2 |
|------|--------|--------|
| Turbo A | X | X |
| Turbo B | Y | Y |

## Zapper Light Gun

Games that support the Zapper light gun (such as Duck Hunt) are operated with the mouse:

| Function | Operation |
|------|------|
| Aim | Move the mouse |
| Fire | Left mouse button |

## Controller API

| API Function | Description |
|----------|------|
| `xnes_controller_joystick_p1(ctl, down, up)` | Player 1 buttons |
| `xnes_controller_joystick_p2(ctl, down, up)` | Player 2 buttons |
| `xnes_controller_joystick_p1_turbo(ctl, down, up)` | Player 1 turbo |
| `xnes_controller_joystick_p2_turbo(ctl, down, up)` | Player 2 turbo |
| `xnes_controller_zapper(ctl, x, y, trigger)` | Zapper light gun |

See [Controller functions](../api/controller) for details.
