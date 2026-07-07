# APU 函数

APU（Audio Processing Unit）是 NES 的音频处理器。libxnes 完整模拟了 APU 的 5 个声道和低通滤波器。

## 声道结构

### 脉冲声道（Pulse 1 & 2）

```c
struct xnes_apu_pulse_t {
    char enabled;           // 启用状态
    uint8_t duty_mode;      // 占空比模式 (0-3)
    uint16_t timer_period;  // 定时器周期
    char sweep_enabled;     // 扫频启用
    char envelope_enabled;  // 包络启用
    uint8_t constant_volume;// 固定音量
    char length_enabled;    // 长度计数器启用
    uint8_t length_value;   // 长度值
    // ...
};
```

两个脉冲声道结构相同，用于生成方波音效。Pulse 1 支持扫频，Pulse 2 不支持。占空比模式有 4 种（12.5%、25%、50%、25% 反相）。

### 三角波声道（Triangle）

```c
struct xnes_apu_triangle_t {
    char enabled;
    uint16_t timer_period;
    uint8_t counter_period; // 线性计数器
    char length_enabled;
    uint8_t length_value;
    // ...
};
```

三角波声道生成 32 步三角波形，常用于低音和节奏。

### 噪声声道（Noise）

```c
struct xnes_apu_noise_t {
    char enabled;
    char mode;              // 模式（周期性/伪随机）
    uint16_t shift_register;// 移位寄存器
    char envelope_enabled;
    uint8_t constant_volume;
    // ...
};
```

噪声声道使用线性反馈移位寄存器生成伪随机噪声，用于打击乐音效。

### DMC 采样声道（DMC）

```c
struct xnes_apu_dmc_t {
    uint16_t sample_address; // 采样起始地址
    uint16_t sample_length;  // 采样长度
    uint16_t current_address;// 当前地址
    uint16_t current_length; // 剩余长度
    char loop;               // 循环播放
    char irq;                // IRQ 启用
    // ...
};
```

DMC 声道直接播放 CPU 内存中的 1-bit Delta 调制音频数据，用于语音和音效采样。

## 函数

### xnes_apu_init

```c
void xnes_apu_init(struct xnes_apu_t * apu, struct xnes_ctx_t * ctx);
```

初始化 APU。清零所有声道状态，设置默认采样率。通常由 `xnes_ctx_alloc` 内部调用。

### xnes_apu_reset

```c
void xnes_apu_reset(struct xnes_apu_t * apu);
```

复位 APU。重置所有声道和帧计数器。

### xnes_apu_step

```c
void xnes_apu_step(struct xnes_apu_t * apu);
```

推进一个 APU 周期。处理声道定时器、包络、扫频、长度计数器，并输出音频采样。每个 CPU 周期调用一次。通常由 `xnes_step_frame` 内部调用。

### xnes_apu_read_register

```c
uint8_t xnes_apu_read_register(struct xnes_apu_t * apu, uint16_t addr);
```

读取 APU 寄存器（$4000-$4017）。

### xnes_apu_write_register

```c
void xnes_apu_write_register(struct xnes_apu_t * apu, uint16_t addr, uint8_t val);
```

写入 APU 寄存器（$4000-$4017）。

### xnes_apu_set_audio_callback

```c
void xnes_apu_set_audio_callback(struct xnes_apu_t * apu, void * data,
    void (*cb)(void *, float), int rate);
```

设置音频输出回调。模拟器在每个音频采样时调用 `cb`，传入用户数据 `data` 和浮点采样值（-1.0 到 1.0）。`rate` 为目标采样率。

通常通过 `xnes_set_audio` 间接调用。

## 音频输出

libxnes 使用浮点采样输出音频，每个采样为 -1.0 到 1.0 范围的单声道值。输出前经过 3 个级联的低通滤波器，模拟真实 NES 的音频特性。

```c
static void audio_cb(void * data, float sample)
{
    /* 将 sample 写入音频缓冲区 */
}
```
