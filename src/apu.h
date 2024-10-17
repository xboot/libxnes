#ifndef __XNES_APU_H__
#define __XNES_APU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnesconf.h>

struct xnes_ctx_t;

struct xnes_apu_pulse_t {
	char enabled;
	uint8_t channel;
	char length_enabled;
	uint8_t length_value;
	uint16_t timer_period;
	uint16_t timer_value;
	uint8_t duty_mode;
	uint8_t duty_value;
	char sweep_reload;
	char sweep_enabled;
	char sweep_negate;
	uint8_t sweep_shift;
	uint8_t sweep_period;
	uint8_t sweep_value;
	char envelope_enabled;
	char envelope_loop;
	char envelope_start;
	uint8_t envelope_period;
	uint8_t envelope_value;
	uint8_t envelope_volume;
	uint8_t constant_volume;
};

struct xnes_apu_triangle_t {
	char enabled;
	char length_enabled;
	uint8_t length_value;
	uint16_t timer_period;
	uint16_t timer_value;
	uint8_t duty_value;
	uint8_t counter_period;
	uint8_t counter_value;
	char counter_reload;
};

struct xnes_apu_noise_t {
	char enabled;
	char mode;
	uint16_t shift_register;
	char length_enabled;
	uint8_t length_value;
	uint16_t timer_period;
	uint16_t timer_value;
	char envelope_enabled;
	char envelope_loop;
	char envelope_start;
	uint8_t envelope_period;
	uint8_t envelope_value;
	uint8_t envelope_volume;
	uint8_t constant_volume;
};

struct xnes_apu_dmc_t {
	struct xnes_ctx_t * ctx;

	uint16_t sample_address;
	uint16_t sample_length;
	uint16_t current_address;
	uint16_t current_length;
	uint8_t shift_register;
	uint8_t bit_count;
	uint8_t tick_period;
	uint8_t tick_value;
	uint8_t value;
	char enabled;
	char loop;
	char irq;
};

struct xnes_apu_filter_t {
    float b0;
    float b1;
    float a1;
    float prevx;
    float prevy;
};

struct xnes_apu_t {
	struct xnes_ctx_t * ctx;

	struct xnes_apu_pulse_t pulse1;
	struct xnes_apu_pulse_t pulse2;
	struct xnes_apu_triangle_t triangle;
	struct xnes_apu_noise_t noise;
	struct xnes_apu_dmc_t dmc;
	struct xnes_apu_filter_t filter[3];
	uint64_t cycles;
	uint8_t frame_period;
	uint8_t frame_value;
	char frame_irq;

	int audio_rate;
	void * audio_data;
	void (*audio_callback)(void * data, float v);
};

void xnes_apu_init(struct xnes_apu_t * apu, struct xnes_ctx_t * ctx);
void xnes_apu_reset(struct xnes_apu_t * apu);
void xnes_apu_step(struct xnes_apu_t * apu);
uint8_t xnes_apu_read_register(struct xnes_apu_t * apu, uint16_t addr);
void xnes_apu_write_register(struct xnes_apu_t * apu, uint16_t addr, uint8_t val);
void xnes_apu_set_audio_callback(struct xnes_apu_t * apu, void * data, void (*cb)(void *, float), int rate);

#ifdef __cplusplus
}
#endif

#endif /* __XNES_APU_H__ */
