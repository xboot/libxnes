# 倒带功能

本示例演示如何使用 libxnes 的状态保存/恢复功能实现倒带（Rewind）。

## 原理

倒带通过持续保存模拟器状态快照实现。游戏时每帧保存状态，倒带时逐帧恢复之前的状态。

```
正向游戏:  step_frame → push → step_frame → push → step_frame → push ...
倒带:      pop → 显示 → pop → 显示 → pop → 显示 ...
```

## 实现

### 1. 创建状态管理器

```c
struct xnes_ctx_t * nes = xnes_ctx_alloc(buf, len);
/* 保存最近 30 秒的状态（60帧/秒 × 30秒 = 1800帧） */
struct xnes_state_t * state = xnes_state_alloc(nes, 60 * 30);
int rewind = 0;
```

### 2. 主循环

```c
uint64_t timestamp = ktime_get();
uint64_t elapsed = 0;

while(running)
{
    /* 处理输入 */
    handle_events(nes, &rewind);

    if(rewind)
    {
        /* 倒带模式：以固定间隔恢复状态 */
        if(ktime_get() - timestamp >= 16666666)  /* 约 60 FPS */
        {
            timestamp = ktime_get();
            elapsed = 16666666;
            xnes_state_pop(state);        /* 恢复上一帧 */
            screen_refresh(nes);           /* 显示画面 */
        }
        else
            SDL_Delay(1);
    }
    else
    {
        /* 正常模式：模拟一帧 */
        if(ktime_get() - timestamp >= elapsed)
        {
            timestamp = ktime_get();
            elapsed = xnes_step_frame(nes);  /* 模拟一帧 */
            screen_refresh(nes);              /* 显示画面 */
            xnes_state_push(state);           /* 保存状态 */
        }
        else
            SDL_Delay(1);
    }
}
```

### 3. 触发倒带

通过键盘 R 键或手柄 L 键触发：

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

### 4. 清理

```c
xnes_state_free(state);
xnes_ctx_free(nes);
```

## 截图

![1943](/screenshots/1943.png)
![circus-chablic](/screenshots/circus-chablic.png)
![lode-runner](/screenshots/lode-runner.png)

:::note
- 状态缓冲区是一个环形队列，当队列满时最旧的状态会被自动丢弃。
- 恢复状态时会自动重置控制器，避免输入状态残留。
- 状态大小取决于卡带的内存配置，通常在几 KB 到几十 KB 之间。
:::
