# APU Functions

The APU (Audio Processing Unit) is the NES's audio processor. libxnes fully emulates the APU's 5 channels and low-pass filter.

## Channel Structure

### Pulse Channels (Pulse 1 & 2)

```c
struct xnes_apu_pulse_t {
    char enabled;           // Enable status
    uint8_t duty_mode;      // Duty cycle mode (0-3)
    uint16_t timer_period;  // Timer period
    char sweep_enabled;     // Sweep enabled
    char envelope_enabled;  // Envelope enabled
    uint8_t constant_volume;// Constant volume
    char length_enabled;    // Length counter enabled
    uint8_t length_value;   // Length value
    // ...
};
```

The two pulse channels share the same structure and are used to generate square wave sound effects. Pulse 1 supports sweep, while Pulse 2 does not. There are 4 duty cycle modes (12.5%, 25%, 50%, 25% inverted).

### Triangle Channel (Triangle)

```c
struct xnes_apu_triangle_t {
    char enabled;
    uint16_t timer_period;
    uint8_t counter_period; // Linear counter
    char length_enabled;
    uint8_t length_value;
    // ...
};
```

The triangle channel generates a 32-step triangular waveform, commonly used for bass and rhythm.

### Noise Channel (Noise)

```c
struct xnes_apu_noise_t {
    char enabled;
    char mode;              // Mode (periodic/pseudo-random)
    uint16_t shift_register;// Shift register
    char envelope_enabled;
    uint8_t constant_volume;
    // ...
};
```

The noise channel uses a linear feedback shift register to generate pseudo-random noise, used for percussion sound effects.

### DMC Sample Channel (DMC)

```c
struct xnes_apu_dmc_t {
    uint16_t sample_address; // Sample start address
    uint16_t sample_length;  // Sample length
    uint16_t current_address;// Current address
    uint16_t current_length; // Remaining length
    char loop;               // Loop playback
    char irq;                // IRQ enabled
    // ...
};
```

The DMC channel directly plays 1-bit Delta modulated audio data from CPU memory, used for voice and sound effect samples.

## Functions

### xnes_apu_init

```c
void xnes_apu_init(struct xnes_apu_t * apu, struct xnes_ctx_t * ctx);
```

Initializes the APU. Zeroes all channel states and sets the default sample rate. Usually called internally by `xnes_ctx_alloc`.

### xnes_apu_reset

```c
void xnes_apu_reset(struct xnes_apu_t * apu);
```

Resets the APU. Resets all channels and the frame counter.

### xnes_apu_step

```c
void xnes_apu_step(struct xnes_apu_t * apu);
```

Advances one APU cycle. Processes channel timers, envelopes, sweeps, length counters, and outputs audio samples. Called once per CPU cycle. Usually called internally by `xnes_step_frame`.

### xnes_apu_read_register

```c
uint8_t xnes_apu_read_register(struct xnes_apu_t * apu, uint16_t addr);
```

Reads an APU register ($4000-$4017).

### xnes_apu_write_register

```c
void xnes_apu_write_register(struct xnes_apu_t * apu, uint16_t addr, uint8_t val);
```

Writes an APU register ($4000-$4017).

### xnes_apu_set_audio_callback

```c
void xnes_apu_set_audio_callback(struct xnes_apu_t * apu, void * data,
    void (*cb)(void *, float), int rate);
```

Sets the audio output callback. The emulator calls `cb` at each audio sample, passing the user data `data` and a floating-point sample value (-1.0 to 1.0). `rate` is the target sample rate.

Usually called indirectly via `xnes_set_audio`.

## Audio Output

libxnes outputs audio using floating-point samples, with each sample being a mono value in the range -1.0 to 1.0. The output passes through 3 cascaded low-pass filters, simulating the audio characteristics of a real NES.

```c
static void audio_cb(void * data, float sample)
{
    /* Write sample to audio buffer */
}
```
