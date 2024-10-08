#ifndef __XNES_CORE_H__
#define __XNES_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#if 1
#define XNES_DEBUG(...)		do{printf(__VA_ARGS__);}while(0)
#else
#define XNES_DEBUG(...)
#endif

struct xnes_ctx_t;

struct xnes_cpu_t {
	struct xnes_ctx_t * ctx;

	uint8_t ram[2048];
	uint64_t cycles;
	uint32_t stall;
	uint16_t pc;
	uint8_t sp;
	uint8_t a;
	uint8_t x;
	uint8_t y;
	union {
		struct {
			uint8_t c :1;
			uint8_t z :1;
			uint8_t i :1;
			uint8_t d :1;
			uint8_t b :1;
			uint8_t u :1;
			uint8_t v :1;
			uint8_t n :1;
		};
		uint8_t p;
	};
	uint8_t interrupt;
};

struct xnes_ppu_t {
	struct xnes_ctx_t * ctx;

	uint32_t palette[64];

	int cycle;
	int scanline;
	uint64_t frame;

	uint8_t palette_data[32];
	uint8_t name_table_data[2048];
	uint8_t oam_data[256];
	uint32_t front_buf[256 * 240];
	uint32_t back_buf[256 * 240];
	uint32_t * front;
	uint32_t * back;

	uint16_t v;
	uint16_t t;
	uint8_t x;
	uint8_t w;
	uint8_t f;

	uint8_t reg;

	uint8_t nmi_occurred;
	uint8_t nmi_output;
	uint8_t nmi_previous;
	uint8_t nmi_delay;

	uint8_t name_table_byte;
	uint8_t attributeTableByte;
	uint8_t low_tile_byte;
	uint8_t high_tile_byte;
	uint64_t tile_data;

	int sprite_count;
	uint32_t sprite_patterns[8];
	uint8_t sprite_positions[8];
	uint8_t sprite_priorities[8];
	uint8_t sprite_indexes[8];

	uint8_t flag_name_table;
	uint8_t flag_increment;
	uint8_t flag_sprite_table;
	uint8_t flag_background_table;
	uint8_t flag_sprite_size;
	uint8_t flag_master_slave;

	uint8_t flag_grayscale;
	uint8_t flag_show_left_background;
	uint8_t flag_show_left_sprites;
	uint8_t flag_show_background;
	uint8_t flag_show_sprites;
	uint8_t flag_red_tint;
	uint8_t flag_green_tint;
	uint8_t flag_blue_tint;

	uint8_t flag_sprite_zero_hit;
	uint8_t flag_sprite_overflow;

	uint8_t oam_address;

	uint8_t buffered_data;
};

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
	void * audio_ctx;
	void (*audio_callback)(void * ctx, float v);
};

enum {
	XNES_JOYSTICK_A			= (1 << 7),
	XNES_JOYSTICK_B			= (1 << 6),
	XNES_JOYSTICK_SELECT	= (1 << 5),
	XNES_JOYSTICK_START		= (1 << 4),
	XNES_JOYSTICK_UP		= (1 << 3),
	XNES_JOYSTICK_DOWN		= (1 << 2),
	XNES_JOYSTICK_LEFT		= (1 << 1),
	XNES_JOYSTICK_RIGHT		= (1 << 0),
};

struct xnes_controller_t {
	struct xnes_ctx_t * ctx;

	uint8_t strobe;
	uint8_t p1;
	uint8_t p1_turbo;
	uint8_t p1_index;
	uint8_t p2;
	uint8_t p2_turbo;
	uint8_t p2_index;
	uint8_t x, y;
	uint8_t trigger;
};

enum {
	XNES_CARTRIDGE_MIRROR_HORIZONTAL	= 0,
	XNES_CARTRIDGE_MIRROR_VERTICAL		= 1,
	XNES_CARTRIDGE_MIRROR_FOUR_SCREEN	= 2,
	XNES_CARTRIDGE_MIRROR_SINGLE0		= 3,
	XNES_CARTRIDGE_MIRROR_SINGLE1		= 4,
};

enum {
	XNES_CARTRIDGE_TIMING_NTSC			= 0,
	XNES_CARTRIDGE_TIMING_PAL			= 1,
	XNES_CARTRIDGE_TIMING_DENDY			= 2,
};

struct xnes_cartridge_t {
	struct xnes_ctx_t * ctx;

	struct {
		uint8_t magic[4];
		uint8_t prg_rom_size_lsb;
		uint8_t chr_rom_size_lsb;
		uint8_t flags_6;
		uint8_t flags_7;
		uint8_t mapper_msb_submapper;
		uint8_t prg_chr_rom_size_msb;
		uint8_t prg_ram_shift_count;
		uint8_t chr_ram_shift_count;
		uint8_t cpu_ppu_timing;
		uint8_t type;
		uint8_t misc;
		uint8_t expansion;
	} header;

	uint16_t mapper_number;
	uint8_t mirror;
	uint8_t timing;
	uint32_t cpu_rate;
	uint32_t cpu_rate_adjusted;
	uint32_t cpu_period_adjusted;

	uint8_t * trainer;
	uint8_t * prg_rom;
	uint8_t * prg_ram;
	uint8_t * prg_nvram;
	uint8_t * chr_rom;
	uint8_t * chr_ram;
	uint8_t * chr_nvram;
	uint32_t trainer_size;
	uint32_t prg_rom_size;
	uint32_t prg_ram_size;
	uint32_t prg_nvram_size;
	uint32_t chr_rom_size;
	uint32_t chr_ram_size;
	uint32_t chr_nvram_size;

	uint8_t sram[8192];

	struct {
		uint8_t (*read)(struct xnes_ctx_t * ctx, uint16_t addr);
		void (*write)(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val);
		void (*step)(struct xnes_ctx_t * ctx);

		union {
			struct {
			} m0;
			struct {
				uint8_t shift;
				uint8_t control;
				uint8_t prg_mode;
				uint8_t chr_mode;
				uint8_t prg_bank;
				uint8_t chr_bank0;
				uint8_t chr_bank1;
				int prg_offsets[2];
				int chr_offsets[2];
			} m1;
			struct {
				uint8_t prg_banks;
				uint8_t prg_bank0;
				uint8_t prg_bank1;
			} m2;
			struct {
				uint8_t chr_bank;
			} m3;
			struct {
				uint8_t reg;
				uint8_t regs[8];
				uint8_t prg_mode;
				uint8_t chr_mode;
				int prg_offsets[4];
				int chr_offsets[8];
				uint8_t reload;
				uint8_t counter;
				uint8_t irq_enable;
			} m4;
			struct {
				uint8_t prg_bank;
			} m7;
			struct {
				uint8_t chr_bank;
				uint8_t prg_banks;
				uint8_t prg_bank0;
				uint8_t prg_bank1;
			} m225;
		} m;
	} mapper;
};

struct xnes_ctx_t {
	struct xnes_cpu_t cpu;
	struct xnes_ppu_t ppu;
	struct xnes_apu_t apu;
	struct xnes_controller_t ctl;
	struct xnes_cartridge_t * cartridge;
};

#ifdef __cplusplus
}
#endif

#endif /* __XNES_CORE_H__ */
