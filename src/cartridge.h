#ifndef __XNES_CARTRIDGE_H__
#define __XNES_CARTRIDGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnesconf.h>

struct xnes_ctx_t;

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
		uint8_t (*cpu_read)(struct xnes_ctx_t * ctx, uint16_t addr);
		void (*cpu_write)(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val);
		uint8_t (*ppu_read)(struct xnes_ctx_t * ctx, uint16_t addr);
		void (*ppu_write)(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val);
		void (*apu_step)(struct xnes_ctx_t * ctx);
		void (*ppu_step)(struct xnes_ctx_t * ctx);

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
				uint8_t prg_bank;
				uint8_t prg_bank_shift;
			} m15;
			struct {
				uint8_t chr_bank;
				uint8_t prg_bank;
			} m66;
			struct {
				uint8_t chr_bank;
				uint8_t prg_bank;
			} m79;
			struct {
				uint8_t chr_bank;
			} m87;
			struct {
				uint8_t chr_bank;
				uint8_t prg_bank;
			} m113;
			struct {
				uint8_t chr_bank;
				uint8_t prg_bank;
			} m140;
			struct {
				uint8_t prg_bank;
			} m177;
			struct {
				uint8_t chr_bank;
				uint8_t prg_banks;
				uint8_t prg_bank0;
				uint8_t prg_bank1;
			} m225;
			struct {
				uint8_t prg_bank;
			} m241;
		} m;
	} mapper;
};

struct xnes_cartridge_t * xnes_cartridge_alloc(const void * buf, size_t len, struct xnes_ctx_t * ctx);
void xnes_cartridge_free(struct xnes_cartridge_t * c);
uint8_t xnes_cartridge_mapper_cpu_read(struct xnes_ctx_t * ctx, uint16_t addr);
void xnes_cartridge_mapper_cpu_write(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val);
uint8_t xnes_cartridge_mapper_ppu_read(struct xnes_ctx_t * ctx, uint16_t addr);
void xnes_cartridge_mapper_ppu_write(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val);
void xnes_cartridge_mapper_apu_step(struct xnes_ctx_t * ctx);
void xnes_cartridge_mapper_ppu_step(struct xnes_ctx_t * ctx);

#ifdef __cplusplus
}
#endif

#endif /* __XNES_CARTRIDGE_H__ */
