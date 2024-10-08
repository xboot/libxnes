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

#include <cpu.h>
#include <mapper.h>

static inline int mapper4_prg_bank_offset(struct xnes_cartridge_t * c, int index)
{
	if(index >= 0x80)
		index -= 0x100;
	index %= c->prg_rom_size / 0x2000;
	int offset = index * 0x2000;
	if(offset < 0)
		offset += c->prg_rom_size;
	return offset;
}

static inline int mapper4_chr_bank_offset(struct xnes_cartridge_t * c, int index)
{
	if(index >= 0x80)
		index -= 0x100;
	index %= c->chr_rom_size / 0x0400;
	int offset = index * 0x0400;
	if(offset < 0)
		offset += c->chr_rom_size;
	return offset;
}

static void mapper4_update_offsets(struct xnes_cartridge_t * c)
{
	switch(c->mapper.m.m4.prg_mode)
	{
	case 0:
		c->mapper.m.m4.prg_offsets[0] = mapper4_prg_bank_offset(c, c->mapper.m.m4.regs[6]);
		c->mapper.m.m4.prg_offsets[1] = mapper4_prg_bank_offset(c, c->mapper.m.m4.regs[7]);
		c->mapper.m.m4.prg_offsets[2] = mapper4_prg_bank_offset(c, -2);
		c->mapper.m.m4.prg_offsets[3] = mapper4_prg_bank_offset(c, -1);
		break;
	case 1:
		c->mapper.m.m4.prg_offsets[0] = mapper4_prg_bank_offset(c, -2);
		c->mapper.m.m4.prg_offsets[1] = mapper4_prg_bank_offset(c, c->mapper.m.m4.regs[7]);
		c->mapper.m.m4.prg_offsets[2] = mapper4_prg_bank_offset(c, c->mapper.m.m4.regs[6]);
		c->mapper.m.m4.prg_offsets[3] = mapper4_prg_bank_offset(c, -1);
		break;
	default:
		break;
	}
	switch(c->mapper.m.m4.chr_mode)
	{
	case 0:
		c->mapper.m.m4.chr_offsets[0] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[0] & 0xfe);
		c->mapper.m.m4.chr_offsets[1] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[0] | 0x01);
		c->mapper.m.m4.chr_offsets[2] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[1] & 0xfe);
		c->mapper.m.m4.chr_offsets[3] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[1] | 0x01);
		c->mapper.m.m4.chr_offsets[4] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[2]);
		c->mapper.m.m4.chr_offsets[5] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[3]);
		c->mapper.m.m4.chr_offsets[6] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[4]);
		c->mapper.m.m4.chr_offsets[7] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[5]);
		break;
	case 1:
		c->mapper.m.m4.chr_offsets[0] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[2]);
		c->mapper.m.m4.chr_offsets[1] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[3]);
		c->mapper.m.m4.chr_offsets[2] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[4]);
		c->mapper.m.m4.chr_offsets[3] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[5]);
		c->mapper.m.m4.chr_offsets[4] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[0] & 0xfe);
		c->mapper.m.m4.chr_offsets[5] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[0] | 0x01);
		c->mapper.m.m4.chr_offsets[6] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[1] & 0xfe);
		c->mapper.m.m4.chr_offsets[7] = mapper4_chr_bank_offset(c, c->mapper.m.m4.regs[1] | 0x01);
		break;
	default:
		break;
	}
}

static uint8_t xnes_mapper4_read(struct xnes_ctx_t * ctx, uint16_t addr)
{
	struct xnes_cartridge_t * c = ctx->cartridge;
	int bank, offset;

	switch(addr >> 13)
	{
	/* PPU address space */
	case 0:	/* [0x0000, 0x1FFF] */
		bank = addr >> 10;
		offset = addr & 0x03ff;
		return c->chr_rom[c->mapper.m.m4.chr_offsets[bank] + offset];
	case 1:	/* [0x2000, 0x3FFF] */
		break;

	/* CPU address space */
	case 2:	/* [0x4000, 0x5FFF] */
		break;
	case 3:	/* [0x6000, 0x7FFF] */
		return c->sram[addr - 0x6000];
	case 4:	/* [0x8000, 0x9FFF] */
	case 5:	/* [0xA000, 0xBFFF] */
	case 6:	/* [0xC000, 0xDFFF] */
	case 7:	/* [0xE000, 0xFFFF] */
		addr -= 0x8000;
		bank = addr >> 13;
		offset = addr & 0x1fff;
		return c->prg_rom[c->mapper.m.m4.prg_offsets[bank] + offset];
	default:
		break;
	}
	return 0;
}

static void xnes_mapper4_write(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val)
{
	struct xnes_cartridge_t * c = ctx->cartridge;
	int bank, offset;

	switch(addr >> 13)
	{
	/* PPU address space */
	case 0:	/* [0x0000, 0x1FFF] */
		bank = addr >> 10;
		offset = addr & 0x03ff;
		c->chr_rom[c->mapper.m.m4.chr_offsets[bank] + offset] = val;
		break;
	case 1:	/* [0x2000, 0x3FFF] */
		break;

	/* CPU address space */
	case 2:	/* [0x4000, 0x5FFF] */
		break;
	case 3:	/* [0x6000, 0x7FFF] */
		c->sram[addr - 0x6000] = val;
		break;
	case 4:	/* [0x8000, 0x9FFF] */
		if(addr & 0x1)
		{
			c->mapper.m.m4.regs[c->mapper.m.m4.reg] = val;
			mapper4_update_offsets(c);
		}
		else
		{
			c->mapper.m.m4.prg_mode = (val >> 6) & 0x1;
			c->mapper.m.m4.chr_mode = (val >> 7) & 0x1;
			c->mapper.m.m4.reg = val & 0x7;
			mapper4_update_offsets(c);
		}
		break;
	case 5:	/* [0xA000, 0xBFFF] */
		if(addr & 0x1)
		{
		}
		else
		{
			if(val & 0x1)
				c->mirror = XNES_CARTRIDGE_MIRROR_HORIZONTAL;
			else
				c->mirror = XNES_CARTRIDGE_MIRROR_VERTICAL;
		}
		break;
	case 6:	/* [0xC000, 0xDFFF] */
		if(addr & 0x1)
			c->mapper.m.m4.counter = 0;
		else
			c->mapper.m.m4.reload = val;
		break;
	case 7:	/* [0xE000, 0xFFFF] */
		if(addr & 0x1)
			c->mapper.m.m4.irq_enable = 1;
		else
			c->mapper.m.m4.irq_enable = 0;
		break;
	default:
		break;
	}
}

static void xnes_mapper4_step(struct xnes_ctx_t * ctx)
{
	struct xnes_ppu_t *ppu = &ctx->ppu;
	struct xnes_cartridge_t *c = ctx->cartridge;

	if(ppu->cycle != 280)
		return;
	if((ppu->scanline > 239) && (ppu->scanline < 261))
		return;
	if((ppu->flag_show_background == 0) && (ppu->flag_show_sprites == 0))
		return;

	if(c->mapper.m.m4.counter == 0)
	{
		c->mapper.m.m4.counter = c->mapper.m.m4.reload;
	}
	else
	{
		c->mapper.m.m4.counter--;
		if((c->mapper.m.m4.counter == 0) && c->mapper.m.m4.irq_enable)
			xnes_cpu_trigger_irq(&ctx->cpu);
	}
}

void xnes_mapper4_init(struct xnes_cartridge_t * c)
{
	c->mapper.m.m4.prg_offsets[0] = mapper4_prg_bank_offset(c, 0);
	c->mapper.m.m4.prg_offsets[1] = mapper4_prg_bank_offset(c, 1);
	c->mapper.m.m4.prg_offsets[2] = mapper4_prg_bank_offset(c, -2);
	c->mapper.m.m4.prg_offsets[3] = mapper4_prg_bank_offset(c, -1);

	c->mapper.read = xnes_mapper4_read;
	c->mapper.write = xnes_mapper4_write;
	c->mapper.step = xnes_mapper4_step;
}
