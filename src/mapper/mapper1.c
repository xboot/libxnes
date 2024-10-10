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

#include <mapper.h>

static inline int mapper1_prg_bank_offset(struct xnes_cartridge_t * c, int index)
{
	if(index >= 0x80)
		index -= 0x100;
	index %= c->prg_rom_size / 0x4000;
	int offset = index * 0x4000;
	if(offset < 0)
		offset += c->prg_rom_size;
	return offset;
}

static inline int mapper1_chr_bank_offset(struct xnes_cartridge_t * c, int index)
{
	if(index >= 0x80)
		index -= 0x100;
	index %= c->chr_rom_size / 0x1000;
	int offset = index * 0x1000;
	if(offset < 0)
		offset += c->chr_rom_size;
	return offset;
}

static void mapper1_update_offsets(struct xnes_cartridge_t * c)
{
	switch(c->mapper.m.m1.prg_mode)
	{
	case 0:
	case 1:
		c->mapper.m.m1.prg_offsets[0] = mapper1_prg_bank_offset(c, c->mapper.m.m1.prg_bank & 0xfe);
		c->mapper.m.m1.prg_offsets[1] = mapper1_prg_bank_offset(c, c->mapper.m.m1.prg_bank | 0x01);
		break;
	case 2:
		c->mapper.m.m1.prg_offsets[0] = 0;
		c->mapper.m.m1.prg_offsets[1] = mapper1_prg_bank_offset(c, c->mapper.m.m1.prg_bank);
		break;
	case 3:
		c->mapper.m.m1.prg_offsets[0] = mapper1_prg_bank_offset(c, c->mapper.m.m1.prg_bank);
		c->mapper.m.m1.prg_offsets[1] = mapper1_prg_bank_offset(c, -1);
		break;
	default:
		break;
	}
	switch(c->mapper.m.m1.chr_mode)
	{
	case 0:
		c->mapper.m.m1.chr_offsets[0] = mapper1_chr_bank_offset(c, c->mapper.m.m1.chr_bank0 & 0xfe);
		c->mapper.m.m1.chr_offsets[1] = mapper1_chr_bank_offset(c, c->mapper.m.m1.chr_bank0 | 0x01);
		break;
	case 1:
		c->mapper.m.m1.chr_offsets[0] = mapper1_chr_bank_offset(c, c->mapper.m.m1.chr_bank0);
		c->mapper.m.m1.chr_offsets[1] = mapper1_chr_bank_offset(c, c->mapper.m.m1.chr_bank1);
		break;
	default:
		break;
	}
}

static void mapper1_write_control(struct xnes_cartridge_t * c, uint8_t val)
{
	c->mapper.m.m1.control = val;
	c->mapper.m.m1.chr_mode = (val >> 4) & 0x1;
	c->mapper.m.m1.prg_mode = (val >> 2) & 0x3;
	switch(val & 0x3)
	{
	case 0:
		c->mirror = XNES_CARTRIDGE_MIRROR_SINGLE0;
		break;
	case 1:
		c->mirror = XNES_CARTRIDGE_MIRROR_SINGLE1;
		break;
	case 2:
		c->mirror = XNES_CARTRIDGE_MIRROR_VERTICAL;
		break;
	case 3:
		c->mirror = XNES_CARTRIDGE_MIRROR_HORIZONTAL;
		break;
	default:
		break;
	}
	mapper1_update_offsets(c);
}

static void mapper1_write_chr_bank0(struct xnes_cartridge_t * c, uint8_t val)
{
	c->mapper.m.m1.chr_bank0 = val;
	mapper1_update_offsets(c);
}

static void mapper1_write_chr_bank1(struct xnes_cartridge_t * c, uint8_t val)
{
	c->mapper.m.m1.chr_bank1 = val;
	mapper1_update_offsets(c);
}

static void mapper1_write_prg_bank(struct xnes_cartridge_t * c, uint8_t val)
{
	c->mapper.m.m1.prg_bank = val & 0xf;
	mapper1_update_offsets(c);
}

static void mapper1_write_register(struct xnes_cartridge_t * c, uint16_t addr, uint8_t val)
{
	switch(addr)
	{
	case 0x0000 ... 0x9fff:
		mapper1_write_control(c, val);
		break;
	case 0xa000 ... 0xbfff:
		mapper1_write_chr_bank0(c, val);
		break;
	case 0xc000 ... 0xdfff:
		mapper1_write_chr_bank1(c, val);
		break;
	case 0xe000 ... 0xffff:
		mapper1_write_prg_bank(c, val);
		break;
	default:
		break;
	}
}

static void mapper1_load_register(struct xnes_cartridge_t * c, uint16_t addr, uint8_t val)
{
	if((val & 0x80) == 0x80)
	{
		c->mapper.m.m1.shift = 0x10;
		mapper1_write_control(c, c->mapper.m.m1.control | 0x0c);
	}
	else
	{
		int complete = c->mapper.m.m1.shift & 0x1;
		c->mapper.m.m1.shift >>= 1;
		c->mapper.m.m1.shift |= (val & 1) << 4;
		if(complete)
		{
			mapper1_write_register(c, addr, c->mapper.m.m1.shift);
			c->mapper.m.m1.shift = 0x10;
		}
	}
}

static uint8_t xnes_mapper1_cpu_read(struct xnes_ctx_t * ctx, uint16_t addr)
{
	struct xnes_cartridge_t * c = ctx->cartridge;
	int bank, offset;

	switch(addr >> 13)
	{
	case 0:	/* [0x0000, 0x1FFF] */
		break;
	case 1:	/* [0x2000, 0x3FFF] */
		break;
	case 2:	/* [0x4000, 0x5FFF] */
		break;
	case 3:	/* [0x6000, 0x7FFF] */
		return c->sram[addr - 0x6000];
	case 4:	/* [0x8000, 0x9FFF] */
	case 5:	/* [0xA000, 0xBFFF] */
	case 6:	/* [0xC000, 0xDFFF] */
	case 7:	/* [0xE000, 0xFFFF] */
		addr -= 0x8000;
		bank = addr >> 14;
		offset = addr & 0x3fff;
		return c->prg_rom[c->mapper.m.m1.prg_offsets[bank] + offset];
	default:
		break;
	}
	return 0;
}

static void xnes_mapper1_cpu_write(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val)
{
	struct xnes_cartridge_t * c = ctx->cartridge;

	switch(addr >> 13)
	{
	case 0:	/* [0x0000, 0x1FFF] */
		break;
	case 1:	/* [0x2000, 0x3FFF] */
		break;
	case 2:	/* [0x4000, 0x5FFF] */
		break;
	case 3:	/* [0x6000, 0x7FFF] */
		c->sram[addr - 0x6000] = val;
		break;
	case 4:	/* [0x8000, 0x9FFF] */
	case 5:	/* [0xA000, 0xBFFF] */
	case 6:	/* [0xC000, 0xDFFF] */
	case 7:	/* [0xE000, 0xFFFF] */
		mapper1_load_register(c, addr, val);
		break;
	default:
		break;
	}
}

static uint8_t xnes_mapper1_ppu_read(struct xnes_ctx_t * ctx, uint16_t addr)
{
	struct xnes_cartridge_t * c = ctx->cartridge;
	int bank, offset;

	switch(addr >> 13)
	{
	case 0:	/* [0x0000, 0x1FFF] */
		bank = addr >> 12;
		offset = addr & 0x0fff;
		return c->chr_rom[c->mapper.m.m1.chr_offsets[bank] + offset];
	case 1:	/* [0x2000, 0x3FFF] */
		break;
	case 2:	/* [0x4000, 0x5FFF] */
		break;
	case 3:	/* [0x6000, 0x7FFF] */
		break;
	case 4:	/* [0x8000, 0x9FFF] */
		break;
	case 5:	/* [0xA000, 0xBFFF] */
		break;
	case 6:	/* [0xC000, 0xDFFF] */
		break;
	case 7:	/* [0xE000, 0xFFFF] */
		break;
	default:
		break;
	}
	return 0;
}

static void xnes_mapper1_ppu_write(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val)
{
	struct xnes_cartridge_t * c = ctx->cartridge;
	int bank, offset;

	switch(addr >> 13)
	{
	case 0:	/* [0x0000, 0x1FFF] */
		bank = addr >> 12;
		offset = addr & 0x0fff;
		c->chr_rom[c->mapper.m.m1.chr_offsets[bank] + offset] = val;
		break;
	case 1:	/* [0x2000, 0x3FFF] */
		break;
	case 2:	/* [0x4000, 0x5FFF] */
		break;
	case 3:	/* [0x6000, 0x7FFF] */
		break;
	case 4:	/* [0x8000, 0x9FFF] */
		break;
	case 5:	/* [0xA000, 0xBFFF] */
		break;
	case 6:	/* [0xC000, 0xDFFF] */
		break;
	case 7:	/* [0xE000, 0xFFFF] */
		break;
	default:
		break;
	}
}

void xnes_mapper1_init(struct xnes_cartridge_t * c)
{
	c->mapper.cpu_read = xnes_mapper1_cpu_read;
	c->mapper.cpu_write = xnes_mapper1_cpu_write;
	c->mapper.ppu_read = xnes_mapper1_ppu_read;
	c->mapper.ppu_write = xnes_mapper1_ppu_write;
	c->mapper.apu_step = NULL;
	c->mapper.ppu_step = NULL;
}
