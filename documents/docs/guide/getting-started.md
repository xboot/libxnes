# 快速开始

本节通过一个简单示例，帮助你在几分钟内上手 libxnes。

## 前置准备

1. 已按[编译安装](./build-guide)完成 libxnes 的编译；
2. 准备一个 `.nes` 格式的 ROM 文件。

## 第一步：加载 ROM

读取 ROM 文件数据，创建模拟器上下文：

```c
#include <xnes.h>

/* 读取 ROM 文件到内存 */
FILE * f = fopen("game.nes", "rb");
fseek(f, 0, SEEK_END);
size_t len = ftell(f);
fseek(f, 0, SEEK_SET);
void * buf = malloc(len);
fread(buf, 1, len, f);
fclose(f);

/* 创建模拟器上下文 */
struct xnes_ctx_t * nes = xnes_ctx_alloc(buf, len);
free(buf);
```

## 第二步：运行一帧

调用 `xnes_step_frame` 推进一帧模拟，返回该帧消耗的纳秒数：

```c
uint64_t elapsed = xnes_step_frame(nes);
```

## 第三步：读取像素

通过 `xnes_get_pixel` 读取 256×240 分辨率的像素数据：

```c
for(int y = 0; y < 240; y++)
{
    for(int x = 0; x < 256; x++)
    {
        uint32_t color = xnes_get_pixel(nes, x, y);
        /* color 为 0xAARRGGBB 格式的 RGBA 值 */
    }
}
```

## 第四步：设置音频

注册音频回调函数，接收模拟器输出的音频采样：

```c
static void audio_callback(void * data, float sample)
{
    /* 将 sample 写入音频设备 */
}

xnes_set_audio(nes, NULL, audio_callback, 44100);
```

## 第五步：处理输入

通过控制器函数设置手柄按键状态：

```c
/* 玩家 1 按下 A 键 */
xnes_controller_joystick_p1(&nes->ctl, XNES_JOYSTICK_A, 0);

/* 玩家 1 松开 A 键 */
xnes_controller_joystick_p1(&nes->ctl, 0, XNES_JOYSTICK_A);
```

## 第六步：销毁资源

```c
xnes_ctx_free(nes);
```

## 完整示例

参见 [Linux SDL2 示例](../examples/basic-usage)，展示了完整的窗口创建、帧循环、音频输出和输入处理。

## 更多操作

- [API 参考](../api/architecture)：了解所有可用函数的详细说明；
- [使用示例](../examples/basic-usage)：查看各类功能的完整代码；
- [参考列表](../reference/mappers)：查看支持的映射器列表和按键映射。
