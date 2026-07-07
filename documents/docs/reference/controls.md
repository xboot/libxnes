# 控制参考

## 键盘映射

| 功能 | 玩家 1 | 玩家 2 |
|------|--------|--------|
| 上 (UP) | W | Arrow Up |
| 下 (DOWN) | S | Arrow Down |
| 左 (LEFT) | A | Arrow Left |
| 右 (RIGHT) | D | Arrow Right |
| A 键 | K | Numpad 6 |
| B 键 | J | Numpad 5 |
| SELECT | Space | Numpad 8 |
| START | Enter | Numpad 9 |

| 功能 | 按键 |
|------|------|
| RESET | ESC |
| 慢速 (SLOW SPEED) | F1 |
| 快速 (FAST SPEED) | F2 |
| 倒带 (REWIND) | R |

## 手柄映射

| 功能 | 手柄 1 | 手柄 2 |
|------|--------|--------|
| 上 (UP) | Up | Up |
| 下 (DOWN) | Down | Down |
| 左 (LEFT) | Left | Left |
| 右 (RIGHT) | Right | Right |
| A 键 | A | A |
| B 键 | B | B |
| SELECT | Select | Select |
| START | Start | Start |
| 倒带 (REWIND) | L | L |
| 快速 (FAST SPEED) | R | R |

## 手柄连发

| 功能 | 手柄 1 | 手柄 2 |
|------|--------|--------|
| 连发 A | X | X |
| 连发 B | Y | Y |

## Zapper 光枪

支持 Zapper 光枪的游戏（如 Duck Hunt）使用鼠标操作：

| 功能 | 操作 |
|------|------|
| 瞄准 | 移动鼠标 |
| 射击 | 鼠标左键 |

## 控制器 API

| API 函数 | 说明 |
|----------|------|
| `xnes_controller_joystick_p1(ctl, down, up)` | 玩家 1 按键 |
| `xnes_controller_joystick_p2(ctl, down, up)` | 玩家 2 按键 |
| `xnes_controller_joystick_p1_turbo(ctl, down, up)` | 玩家 1 连发 |
| `xnes_controller_joystick_p2_turbo(ctl, down, up)` | 玩家 2 连发 |
| `xnes_controller_zapper(ctl, x, y, trigger)` | Zapper 光枪 |

详见 [Controller 函数](../api/controller)。
