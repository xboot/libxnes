# 音频输出

本示例演示如何设置 libxnes 的音频输出。

## 设置音频回调

通过 `xnes_set_audio` 注册音频回调函数：

```c
struct xnes_ctx_t * nes = xnes_ctx_alloc(buf, len);

/* 音频回调函数 */
static void audio_callback(void * data, float sample)
{
    /* sample 范围: -1.0 ~ 1.0 (单声道浮点) */
    /* 将采样写入音频设备缓冲区 */
}

xnes_set_audio(nes, user_data, audio_callback, 44100);
```

参数说明：
- `data` — 传递给回调函数的用户数据指针
- `cb` — 音频回调函数，每个采样调用一次
- `rate` — 目标采样率（如 44100 或 48000）

## SDL2 音频集成

使用 SDL2 的音频设备播放模拟器音频：

```c
/* 打开 SDL 音频设备 */
SDL_AudioSpec wantspec = {0};
wantspec.freq = 48000;
wantspec.format = AUDIO_F32SYS;
wantspec.channels = 2;
wantspec.samples = 2048;
wantspec.callback = NULL;
SDL_AudioDeviceID audio = SDL_OpenAudioDevice(0, 0, &wantspec, &havespec,
    SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_SAMPLES_CHANGE);
SDL_PauseAudioDevice(audio, 0);

/* 设置音频回调（双声道输出） */
static void audio_cb(void * data, float sample)
{
    SDL_AudioDeviceID audio = *(SDL_AudioDeviceID *)data;
    /* 单声道转双声道 */
    SDL_QueueAudio(audio, &sample, sizeof(float));
    SDL_QueueAudio(audio, &sample, sizeof(float));
}

xnes_set_audio(nes, &audio, audio_cb, havespec.freq);
```

:::note
libxnes 输出单声道浮点采样。如需双声道输出，将同一个采样分别写入左右声道。
:::

## 重新加载 ROM 时重置音频

```c
SDL_ClearQueuedAudio(audio);
xnes_set_audio(nes, &audio, audio_cb, havespec.freq);
```
