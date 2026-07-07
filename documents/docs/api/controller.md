# Controller 函数

libxnes 支持标准的 NES 控制器输入，包括 2 个手柄、Zapper 光枪和连发（Turbo）功能。

## 手柄按键

```c
enum {
    XNES_JOYSTICK_A      = (1 << 7), // A 键
    XNES_JOYSTICK_B      = (1 << 6), // B 键
    XNES_JOYSTICK_SELECT = (1 << 5), // Select 键
    XNES_JOYSTICK_START  = (1 << 4), // Start 键
    XNES_JOYSTICK_UP     = (1 << 3), // 上
    XNES_JOYSTICK_DOWN   = (1 << 2), // 下
    XNES_JOYSTICK_LEFT   = (1 << 1), // 左
    XNES_JOYSTICK_RIGHT  = (1 << 0), // 右
};
```

## 连发速度

```c
enum {
    XNES_CONTROLLER_TURBO_SPEED_FAST   = (1 << 1), // 快速连发
    XNES_CONTROLLER_TURBO_SPEED_NORMAL = (1 << 2), // 正常连发
    XNES_CONTROLLER_TURBO_SPEED_SLOW   = (1 << 3), // 慢速连发
};
```

## 数据结构

```c
struct xnes_controller_t {
    struct xnes_ctx_t * ctx;
    uint8_t turbo_count;  // 连发计数器
    uint8_t turbo_speed;  // 连发速度
    uint8_t latch;        // 移位寄存器锁存
    struct {
        struct { uint8_t key, key_turbo, key_index; } p1;
        struct { uint8_t key, key_turbo, key_index; } p2;
    } joystick;
    struct { uint8_t x, y, trigger; } zapper; // Zapper 光枪
};
```

## 函数

### xnes_controller_init

```c
void xnes_controller_init(struct xnes_controller_t * ctl, struct xnes_ctx_t * ctx);
```

初始化控制器。清零所有状态，设置默认连发速度。通常由 `xnes_ctx_alloc` 内部调用。

### xnes_controller_reset

```c
void xnes_controller_reset(struct xnes_controller_t * ctl);
```

复位控制器，清零所有按键状态。

### xnes_controller_set_turbo_speed

```c
void xnes_controller_set_turbo_speed(struct xnes_controller_t * ctl, uint8_t speed);
```

设置连发速度。使用 `XNES_CONTROLLER_TURBO_SPEED_*` 宏的组合。

### xnes_controller_joystick_p1 / p2

```c
void xnes_controller_joystick_p1(struct xnes_controller_t * ctl, uint8_t down, uint8_t up);
void xnes_controller_joystick_p2(struct xnes_controller_t * ctl, uint8_t down, uint8_t up);
```

设置玩家 1 或玩家 2 的手柄按键状态。`down` 是本次按下的按键掩码，`up` 是本次松开的按键掩码。使用 `XNES_JOYSTICK_*` 宏的组合。

```c
/* 玩家 1 按下 A 键和右方向键 */
xnes_controller_joystick_p1(&ctx->ctl, XNES_JOYSTICK_A | XNES_JOYSTICK_RIGHT, 0);

/* 玩家 1 松开 A 键 */
xnes_controller_joystick_p1(&ctx->ctl, 0, XNES_JOYSTICK_A);
```

### xnes_controller_joystick_p1_turbo / p2_turbo

```c
void xnes_controller_joystick_p1_turbo(struct xnes_controller_t * ctl, uint8_t down, uint8_t up);
void xnes_controller_joystick_p2_turbo(struct xnes_controller_t * ctl, uint8_t down, uint8_t up);
```

设置连发按键状态。按住时会以连发速度自动重复按下/松开。参数同上。

### xnes_controller_zapper

```c
void xnes_controller_zapper(struct xnes_controller_t * ctl, uint8_t x, uint8_t y, uint8_t trigger);
```

设置 Zapper 光枪状态。`x` 和 `y` 是屏幕坐标（0-255, 0-239），`trigger` 非零表示扣动扳机。模拟器内部通过检查 PPU 对应位置的像素亮度判断是否命中。

```c
/* 扣动扳机 */
xnes_controller_zapper(&ctx->ctl, 128, 120, 1);

/* 松开扳机 */
xnes_controller_zapper(&ctx->ctl, 128, 120, 0);
```

### xnes_controller_read_register / write_register

```c
uint8_t xnes_controller_read_register(struct xnes_controller_t * ctl, uint16_t addr);
void xnes_controller_write_register(struct xnes_controller_t * ctl, uint16_t addr, uint8_t val);
```

读写控制器寄存器（$4016/$4017）。由 CPU 内部调用，通常不需要直接使用。
