# Audio Output

This example demonstrates how to set up libxnes audio output.

## Setting the audio callback

Register an audio callback function via `xnes_set_audio`:

```c
struct xnes_ctx_t * nes = xnes_ctx_alloc(buf, len);

/* Audio callback function */
static void audio_callback(void * data, float sample)
{
    /* sample range: -1.0 ~ 1.0 (mono floating point) */
    /* Write the sample to the audio device buffer */
}

xnes_set_audio(nes, user_data, audio_callback, 44100);
```

Parameter description:
- `data` — user data pointer passed to the callback function
- `cb` — audio callback function, called once per sample
- `rate` — target sample rate (e.g. 44100 or 48000)

## SDL2 audio integration

Use SDL2's audio device to play the emulator audio:

```c
/* Open the SDL audio device */
SDL_AudioSpec wantspec = {0};
wantspec.freq = 48000;
wantspec.format = AUDIO_F32SYS;
wantspec.channels = 2;
wantspec.samples = 2048;
wantspec.callback = NULL;
SDL_AudioDeviceID audio = SDL_OpenAudioDevice(0, 0, &wantspec, &havespec,
    SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_SAMPLES_CHANGE);
SDL_PauseAudioDevice(audio, 0);

/* Set the audio callback (stereo output) */
static void audio_cb(void * data, float sample)
{
    SDL_AudioDeviceID audio = *(SDL_AudioDeviceID *)data;
    /* Mono to stereo */
    SDL_QueueAudio(audio, &sample, sizeof(float));
    SDL_QueueAudio(audio, &sample, sizeof(float));
}

xnes_set_audio(nes, &audio, audio_cb, havespec.freq);
```

:::note
libxnes outputs mono floating-point samples. For stereo output, write the same sample to the left and right channels separately.
:::

## Reset audio when reloading the ROM

```c
SDL_ClearQueuedAudio(audio);
xnes_set_audio(nes, &audio, audio_cb, havespec.freq);
```
