# Context 函数

模拟器上下文 `xnes_ctx_t` 是 libxnes 的核心对象，包含所有模拟器组件。所有模拟操作都通过上下文完成。

## 生命周期

### xnes_ctx_alloc

```c
struct xnes_ctx_t * xnes_ctx_alloc(const void * buf, size_t len);
```

从 ROM 数据创建模拟器上下文。`buf` 是 `.nes` 文件的数据，`len` 是数据长度。函数内部会解析 ROM 头（NES 2.0 格式），加载卡带数据，并初始化 CPU、DMA、PPU、APU 和控制器。

返回 `NULL` 表示 ROM 解析失败。

### xnes_ctx_free

```c
void xnes_ctx_free(struct xnes_ctx_t * ctx);
```

释放模拟器上下文及其所有资源，包括卡带数据。传入 `NULL` 安全。

### xnes_reset

```c
void xnes_reset(struct xnes_ctx_t * ctx);
```

复位模拟器，将 CPU、DMA、PPU、APU 和控制器恢复到初始状态。等同于按下 NES 的复位按钮。

## 模拟控制

### xnes_step_frame

```c
uint64_t xnes_step_frame(struct xnes_ctx_t * ctx);
```

推进一帧的模拟。内部循环执行 CPU 指令，每个 CPU 周期执行 3 次 PPU 步进和 1 次 APU 步进，直到 PPU 完成一帧（29780 个 CPU 周期）。

**返回值**：该帧消耗的纳秒数（基于 CPU 频率和速度设置），用于帧率控制。

### xnes_set_speed

```c
void xnes_set_speed(struct xnes_ctx_t * ctx, float speed);
```

设置模拟速度。`1.0` 为正常速度，`0.5` 为半速，`2.0` 为两倍速。影响 `xnes_step_frame` 返回的纳秒数。

### xnes_set_debugger

```c
void xnes_set_debugger(struct xnes_ctx_t * ctx, int (*debugger)(struct xnes_ctx_t *));
```

设置调试器回调函数。在每条 CPU 指令执行前调用，如果回调返回非零值，则暂停模拟。传入 `NULL` 取消调试器。

```c
static int my_debugger(struct xnes_ctx_t * ctx)
{
    /* 检查 CPU 状态 */
    if(ctx->cpu.pc == 0xC000)
        return 1; /* 在指定地址暂停 */
    return 0;     /* 继续执行 */
}

xnes_set_debugger(nes, my_debugger);
```

## 输出

### xnes_get_pixel

```c
uint32_t xnes_get_pixel(struct xnes_ctx_t * ctx, int x, int y);
```

获取当前帧指定位置的像素颜色。`x` 范围 0-255，`y` 范围 0-239。返回 `0xAARRGGBB` 格式的 ARGB 值（Alpha 始终为 0xFF）。

```c
for(int y = 0; y < 240; y++)
{
    for(int x = 0; x < 256; x++)
    {
        uint32_t color = xnes_get_pixel(nes, x, y);
        /* 写入 framebuffer */
    }
}
```

### xnes_set_palette

```c
void xnes_set_palette(struct xnes_ctx_t * ctx, uint32_t * pal);
```

设置自定义调色板。`pal` 是 64 个 `uint32_t` 值的数组，格式为 `0xAARRGGBB`。传入 `NULL` 无效。默认使用内置的 NES 标准调色板。

### xnes_set_audio

```c
void xnes_set_audio(struct xnes_ctx_t * ctx, void * data, void (*cb)(void *, float), int rate);
```

设置音频回调。模拟器在每个音频采样时调用 `cb`，`data` 作为回调的第一个参数传入，`float` 参数是音频采样值（-1.0 到 1.0）。`rate` 是目标采样率（如 44100 或 48000）。

```c
static void audio_cb(void * data, float sample)
{
    /* 将 sample 写入音频设备 */
}

xnes_set_audio(nes, my_data, audio_cb, 44100);
```
