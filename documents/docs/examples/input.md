# 输入处理

本示例演示如何处理键盘和手柄输入，将其转换为 libxnes 控制器状态。

## 键盘输入

将键盘事件映射到 NES 手柄按键：

```c
/* 按键按下 */
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

/* 按键松开 */
case SDL_KEYUP:
    switch(e.key.keysym.sym)
    {
    case SDLK_w:
        xnes_controller_joystick_p1(&nes->ctl, 0, XNES_JOYSTICK_UP);
        break;
    /* ... 其他按键类似 ... */
    }
    break;
```

## 手柄输入

将 SDL 手柄事件映射到 NES 控制器：

### 摇杆轴

```c
case SDL_JOYAXISMOTION:
    if(e.jaxis.axis == 0)  /* X 轴 */
    {
        if(e.jaxis.value < -3200)
            xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_LEFT, 0);
        else if(e.jaxis.value > 3200)
            xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_RIGHT, 0);
        else
            xnes_controller_joystick_p1(&nes->ctl, 0, XNES_JOYSTICK_LEFT | XNES_JOYSTICK_RIGHT);
    }
    else if(e.jaxis.axis == 1)  /* Y 轴 */
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

### 手柄按钮

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
    case 2: /* Y - 连发 B */
        xnes_controller_joystick_p1_turbo(&nes->ctl, XNES_JOYSTICK_B, 0);
        break;
    case 3: /* X - 连发 A */
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

## 速度控制

通过 `xnes_set_speed` 控制模拟速度：

```c
case SDLK_F1:
    xnes_set_speed(nes, 0.5);  /* 慢速 */
    break;
case SDLK_F2:
    xnes_set_speed(nes, 2.0);  /* 快速 */
    break;
/* 松开时恢复 */
case SDL_KEYUP:
    if(e.key.keysym.sym == SDLK_F1 || e.key.keysym.sym == SDLK_F2)
        xnes_set_speed(nes, 1.0);  /* 正常速度 */
    break;
```

## Zapper 光枪

对于支持光枪的游戏（如 Duck Hunt），使用鼠标作为光枪：

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

完整按键映射参见[控制参考](../reference/controls)。
