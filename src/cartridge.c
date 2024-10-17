/*
 * Copyright(c) Jianjun Jiang <8192542@qq.com>
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <xnes.h>

extern void xnes_mapper0_init(struct xnes_cartridge_t * c);
extern void xnes_mapper1_init(struct xnes_cartridge_t * c);
extern void xnes_mapper2_init(struct xnes_cartridge_t * c);
extern void xnes_mapper3_init(struct xnes_cartridge_t * c);
extern void xnes_mapper4_init(struct xnes_cartridge_t * c);
extern void xnes_mapper7_init(struct xnes_cartridge_t * c);
extern void xnes_mapper66_init(struct xnes_cartridge_t * c);
extern void xnes_mapper140_init(struct xnes_cartridge_t * c);
extern void xnes_mapper225_init(struct xnes_cartridge_t * c);

static int xnes_cartridge_mapper_init(struct xnes_cartridge_t * c)
{
	if(c)
	{
		switch(c->mapper_number)
		{
		case 0:
			xnes_mapper0_init(c);
			return 1;
		case 1:
			xnes_mapper1_init(c);
			return 1;
		case 2:
			xnes_mapper2_init(c);
			return 1;
		case 3:
			xnes_mapper3_init(c);
			return 1;
		case 4:
			xnes_mapper4_init(c);
			return 1;
		case 7:
			xnes_mapper7_init(c);
			return 1;
		case 66:
			xnes_mapper66_init(c);
			return 1;
		case 140:
			xnes_mapper140_init(c);
			return 1;
		case 225:
			xnes_mapper225_init(c);
			return 1;
		default:
			break;
		}
	}
	return 0;
}

struct xnes_cartridge_t * xnes_cartridge_alloc(const void * buf, size_t len, struct xnes_ctx_t * ctx)
{
	uint8_t * p;

	if(!buf || len <= 16)
		return NULL;

	p = (uint8_t *)buf;
	if((p[0] != 'N') || (p[1] != 'E') || (p[2] != 'S') || (p[3] != 0x1a))
		return NULL;

	struct xnes_cartridge_t * c = xnes_malloc(sizeof(struct xnes_cartridge_t));
	if(!c)
		return NULL;

	xnes_memset(c, 0, sizeof(struct xnes_cartridge_t));
	xnes_memcpy(&c->header, buf, 16);
	c->ctx = ctx;

	if(((c->header.flags_7 >> 2) & 0x3) == 0x2)
	{
		c->mapper_number = (((c->header.mapper_msb_submapper >> 4) & 0x0f) << 8) | (((c->header.flags_7 >> 4) & 0x0f) << 4) | (((c->header.flags_6 >> 4) & 0x0f) << 0);
		if((c->header.flags_6 >> 0) & 0x1)
			c->mirror = XNES_CARTRIDGE_MIRROR_VERTICAL;
		else
			c->mirror = XNES_CARTRIDGE_MIRROR_HORIZONTAL;
		if((c->header.flags_6 >> 3) & 0x1)
			c->mirror = XNES_CARTRIDGE_MIRROR_FOUR_SCREEN;

		p += 16;
		if((c->header.flags_6 >> 2) & 0x1)
		{
			c->trainer_size = 512;
			c->trainer = xnes_malloc(c->trainer_size);
			xnes_memcpy(c->trainer, p, c->trainer_size);
			p += c->trainer_size;
		}
		else
		{
			c->trainer_size = 0;
			c->trainer = NULL;
		}

		c->prg_rom_size = c->header.prg_rom_size_lsb * 16384;
		c->prg_rom = xnes_malloc(c->prg_rom_size);
		xnes_memcpy(c->prg_rom, p, c->prg_rom_size);
		p += c->prg_rom_size;

		if(((c->header.prg_ram_shift_count >> 0) & 0xf) > 0)
		{
			c->prg_ram_size = 64 << ((c->header.prg_ram_shift_count >> 0) & 0xf);
			c->prg_ram = xnes_malloc(c->prg_ram_size);
			xnes_memset(c->prg_ram, 0, c->prg_ram_size);
		}
		else
		{
			c->prg_ram_size = 0;
			c->prg_ram = NULL;
		}

		if(((c->header.prg_ram_shift_count >> 4) & 0xf) > 0)
		{
			c->prg_nvram_size = 64 << ((c->header.prg_ram_shift_count >> 4) & 0xf);
			c->prg_nvram = xnes_malloc(c->prg_nvram_size);
			xnes_memset(c->prg_nvram, 0, c->prg_nvram_size);
		}
		else
		{
			c->prg_nvram_size = 0;
			c->prg_nvram = NULL;
		}

		c->chr_rom_size = c->header.chr_rom_size_lsb * 8192;
		c->chr_rom = xnes_malloc(c->chr_rom_size);
		xnes_memcpy(c->chr_rom, p, c->chr_rom_size);
		p += c->chr_rom_size;

		if(((c->header.chr_ram_shift_count >> 0) & 0xf) > 0 )
		{
			c->chr_ram_size = 64 << ((c->header.chr_ram_shift_count >> 0) & 0xf);
			c->chr_ram = xnes_malloc(c->chr_ram_size);
			xnes_memset(c->chr_ram, 0, c->chr_ram_size);
		}
		else
		{
			c->chr_ram_size = 0;
			c->chr_ram = NULL;
		}

		if(((c->header.chr_ram_shift_count >> 4) & 0xf) > 0 )
		{
			c->chr_nvram_size = 64 << ((c->header.chr_ram_shift_count >> 4) & 0xf);
			c->chr_nvram = xnes_malloc(c->chr_nvram_size);
			xnes_memset(c->chr_nvram, 0, c->chr_nvram_size);
		}
		else
		{
			c->chr_nvram_size = 0;
			c->chr_nvram = NULL;
		}

		switch(c->header.cpu_ppu_timing & 0x3)
		{
		case 0x0:
			c->timing = XNES_CARTRIDGE_TIMING_NTSC;
			c->cpu_rate = 1789773;
			c->cpu_rate_adjusted = 1789773;
			c->cpu_period_adjusted = 559;
			break;

		case 0x1:
			c->timing = XNES_CARTRIDGE_TIMING_PAL;
			c->cpu_rate = 1662607;
			c->cpu_rate_adjusted = 1662607;
			c->cpu_period_adjusted = 601;
			break;

		case 0x3:
			c->timing = XNES_CARTRIDGE_TIMING_DENDY;
			c->cpu_rate = 1773448;
			c->cpu_rate_adjusted = 1773448;
			c->cpu_period_adjusted = 564;
			break;

		default:
			c->timing = XNES_CARTRIDGE_TIMING_NTSC;
			c->cpu_rate = 1789773;
			c->cpu_rate_adjusted = 1789773;
			c->cpu_period_adjusted = 559;
			break;
		}
	}
	else
	{
		c->mapper_number = (((c->header.flags_7 >> 4) & 0x0f) << 4) | (((c->header.flags_6 >> 4) & 0x0f) << 0);
		if((c->header.flags_6 >> 0) & 0x1)
			c->mirror = XNES_CARTRIDGE_MIRROR_VERTICAL;
		else
			c->mirror = XNES_CARTRIDGE_MIRROR_HORIZONTAL;
		if((c->header.flags_6 >> 3) & 0x1)
			c->mirror = XNES_CARTRIDGE_MIRROR_FOUR_SCREEN;

		p += 16;
		if((c->header.flags_6 >> 2) & 0x1)
		{
			c->trainer_size = 512;
			c->trainer = xnes_malloc(c->trainer_size);
			xnes_memcpy(c->trainer, p, c->trainer_size);
			p += c->trainer_size;
		}
		else
		{
			c->trainer_size = 0;
			c->trainer = NULL;
		}

		c->prg_rom_size = c->header.prg_rom_size_lsb * 16384;
		c->prg_rom = xnes_malloc(c->prg_rom_size);
		xnes_memcpy(c->prg_rom, p, c->prg_rom_size);
		p += c->prg_rom_size;

		c->prg_ram_size = 0;
		c->prg_ram = NULL;

		c->prg_nvram_size = 0;
		c->prg_nvram = NULL;

		c->chr_rom_size = c->header.chr_rom_size_lsb * 8192;
		if(c->chr_rom_size == 0)
			c->chr_rom_size = 8192;
		c->chr_rom = xnes_malloc(c->chr_rom_size);
		xnes_memcpy(c->chr_rom, p, c->chr_rom_size);
		p += c->chr_rom_size;

		c->chr_ram_size = 0;
		c->chr_ram = NULL;

		c->chr_nvram_size = 0;
		c->chr_nvram = NULL;

		if((c->header.prg_chr_rom_size_msb >> 0) & 0x1)
		{
			c->timing = XNES_CARTRIDGE_TIMING_PAL;
			c->cpu_rate_adjusted = 1662607;
			c->cpu_period_adjusted = 601;
		}
		else
		{
			c->timing = XNES_CARTRIDGE_TIMING_NTSC;
			c->cpu_rate = 1789773;
			c->cpu_rate_adjusted = 1789773;
			c->cpu_period_adjusted = 559;
		}
	}

	if(!xnes_cartridge_mapper_init(c))
	{
		xnes_free(c);
		return NULL;
	}
	return c;
}

void xnes_cartridge_free(struct xnes_cartridge_t * c)
{
	if(c)
	{
		if(c->trainer)
			xnes_free(c->trainer);
		if(c->prg_rom)
			xnes_free(c->prg_rom);
		if(c->chr_rom)
			xnes_free(c->chr_rom);
		if(c->prg_ram)
			xnes_free(c->prg_ram);
		if(c->chr_ram)
			xnes_free(c->chr_ram);
		xnes_free(c);
	}
}

uint8_t xnes_cartridge_mapper_cpu_read(struct xnes_ctx_t * ctx, uint16_t addr)
{
	return ctx->cartridge->mapper.cpu_read(ctx, addr);
}

void xnes_cartridge_mapper_cpu_write(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val)
{
	ctx->cartridge->mapper.cpu_write(ctx, addr, val);
}

uint8_t xnes_cartridge_mapper_ppu_read(struct xnes_ctx_t * ctx, uint16_t addr)
{
	return ctx->cartridge->mapper.ppu_read(ctx, addr);
}

void xnes_cartridge_mapper_ppu_write(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val)
{
	ctx->cartridge->mapper.ppu_write(ctx, addr, val);
}

void xnes_cartridge_mapper_apu_step(struct xnes_ctx_t * ctx)
{
	if(ctx->cartridge->mapper.apu_step)
		ctx->cartridge->mapper.apu_step(ctx);
}

void xnes_cartridge_mapper_ppu_step(struct xnes_ctx_t * ctx)
{
	if(ctx->cartridge->mapper.ppu_step)
		ctx->cartridge->mapper.ppu_step(ctx);
}
