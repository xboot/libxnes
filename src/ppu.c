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

#include <cartridge.h>
#include <cpu.h>
#include <ppu.h>

static const uint16_t mirror_lookup[5][4] = {
	{ 0, 0, 1, 1 },
	{ 0, 1, 0, 1 },
	{ 0, 1, 2, 3 },
	{ 0, 0, 0, 0 },
	{ 1, 1, 1, 1 },
};

static inline uint16_t mirror_address(uint8_t mode, uint16_t addr)
{
	addr = (addr - 0x2000) & 0x0fff;
	uint16_t table = addr >> 10;
	uint16_t offset = addr & 0x3ff;
	return 0x2000 + (mirror_lookup[mode][table] << 10) + offset;
}

static inline uint8_t ppu_read_palette(struct xnes_ppu_t * ppu, uint16_t addr)
{
	if((addr >= 16) && ((addr & 0x3) == 0))
		addr -= 16;
	return ppu->palette_data[addr];
}

static inline void ppu_write_palette(struct xnes_ppu_t * ppu, uint16_t addr, uint8_t val)
{
	if((addr >= 16) && ((addr & 0x3) == 0))
		addr -= 16;
	ppu->palette_data[addr] = val;
}

static uint8_t ppu_read8(struct xnes_ppu_t * ppu, uint16_t addr)
{
	struct xnes_ctx_t * ctx = ppu->ctx;

	addr &= 0x3fff;
	switch(addr >> 13)
	{
	/* [0x0000, 0x1FFF] */
	case 0:
		return xnes_cartridge_mapper_ppu_read(ctx, addr);

	/* [0x2000, 0x3FFF] */
	case 1:
		switch(addr)
		{
		case 0x2000 ... 0x3eff:
			return ppu->name_table_data[mirror_address(ctx->cartridge->mirror, addr) & 0x7ff];
		case 0x3f00 ... 0x3fff:
			return ppu_read_palette(ppu, addr & 0x1f);
		default:
			break;
		}
		break;

	default:
		break;
	}
	return 0;
}

static void ppu_write8(struct xnes_ppu_t * ppu, uint16_t addr, uint8_t val)
{
	struct xnes_ctx_t * ctx = ppu->ctx;

	addr &= 0x3fff;
	switch(addr >> 13)
	{
	/* [0x0000, 0x1FFF] */
	case 0:
		xnes_cartridge_mapper_ppu_write(ctx, addr, val);
		break;

	/* [0x2000, 0x3FFF] */
	case 1:
		switch(addr)
		{
		case 0x2000 ... 0x3eff:
			ppu->name_table_data[mirror_address(ctx->cartridge->mirror, addr) & 0x7ff] = val;
			break;
		case 0x3f00 ... 0x3fff:
			ppu_write_palette(ppu, addr & 0x1f, val);
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
}

static void ppu_nmi_change(struct xnes_ppu_t * ppu)
{
	uint8_t nmi = ppu->nmi_output && ppu->nmi_occurred;
	if(nmi && !ppu->nmi_previous)
		ppu->nmi_delay = 15;
	ppu->nmi_previous = nmi;
}

static void ppu_write_control(struct xnes_ppu_t * ppu, uint8_t val)
{
	ppu->flag_name_table = (val >> 0) & 3;
	ppu->flag_increment = (val >> 2) & 1;
	ppu->flag_sprite_table = (val >> 3) & 1;
	ppu->flag_background_table = (val >> 4) & 1;
	ppu->flag_sprite_size = (val >> 5) & 1;
	ppu->flag_master_slave = (val >> 6) & 1;
	ppu->nmi_output = (((val >> 7) & 1) == 1);
	ppu_nmi_change(ppu);
	ppu->t = (ppu->t & 0xF3FF) | (((uint16_t)(val) & 0x03) << 10);
}

static void ppu_write_mask(struct xnes_ppu_t * ppu, uint8_t val)
{
	ppu->flag_grayscale = (val >> 0) & 1;
	ppu->flag_show_left_background = (val >> 1) & 1;
	ppu->flag_show_left_sprites = (val >> 2) & 1;
	ppu->flag_show_background = (val >> 3) & 1;
	ppu->flag_show_sprites = (val >> 4) & 1;
	ppu->flag_red_tint = (val >> 5) & 1;
	ppu->flag_green_tint = (val >> 6) & 1;
	ppu->flag_blue_tint = (val >> 7) & 1;
}

static inline void ppu_write_oam_address(struct xnes_ppu_t * ppu, uint8_t val)
{
	ppu->oam_address = val;
}

static uint8_t ppu_read_status(struct xnes_ppu_t * ppu)
{
	uint8_t result = ppu->reg & 0x1F;
	result |= ppu->flag_sprite_overflow << 5;
	result |= ppu->flag_sprite_zero_hit << 6;
	if(ppu->nmi_occurred)
		result |= 1 << 7;
	ppu->nmi_occurred = 0;
	ppu_nmi_change(ppu);
	ppu->w = 0;
	return result;
}

static uint8_t ppu_read_oam_data(struct xnes_ppu_t * ppu)
{
	uint8_t data = ppu->oam_data[ppu->oam_address];
	if((ppu->oam_address & 0x03) == 0x02)
		data = data & 0xE3;
	return data;
}

static void ppu_write_oam_data(struct xnes_ppu_t * ppu , uint8_t val)
{
	ppu->oam_data[ppu->oam_address] = val;
	ppu->oam_address++;
}

static void ppu_write_scroll(struct xnes_ppu_t * ppu, uint8_t val)
{
	if(ppu->w == 0)
	{
		ppu->t = (ppu->t & 0xFFE0) | ((uint16_t)val >> 3);
		ppu->x = val & 0x07;
		ppu->w = 1;
	}
	else
	{
		ppu->t = (ppu->t & 0x8FFF) | (((uint16_t)val & 0x07) << 12);
		ppu->t = (ppu->t & 0xFC1F) | (((uint16_t)val & 0xF8) << 2);
		ppu->w = 0;
	}
}

static void ppu_write_address(struct xnes_ppu_t * ppu, uint8_t val)
{
	if(ppu->w == 0)
	{
		ppu->t = (ppu->t & 0x80FF) | (((uint16_t)(val) & 0x3F) << 8);
		ppu->w = 1;
	}
	else
	{
		ppu->t = (ppu->t & 0xFF00) | (uint16_t)(val);
		ppu->v = ppu->t;
		ppu->w = 0;
	}
}

static uint8_t ppu_read_data(struct xnes_ppu_t * ppu)
{
	uint8_t val = ppu_read8(ppu, ppu->v);
	if((ppu->v % 0x4000) < 0x3F00)
	{
		uint8_t buffered = ppu->buffered_data;
		ppu->buffered_data = val;
		val = buffered;
	}
	else
		ppu->buffered_data = ppu_read8(ppu, ppu->v - 0x1000);
	if(ppu->flag_increment == 0)
		ppu->v += 1;
	else
		ppu->v += 32;
	return val;
}

static void ppu_write_data(struct xnes_ppu_t * ppu, uint8_t val)
{
	ppu_write8(ppu, ppu->v, val);
	if(ppu->flag_increment == 0)
		ppu->v += 1;
	else
		ppu->v += 32;
}

static void ppu_write_dma(struct xnes_ppu_t * ppu, uint8_t val)
{
	uint16_t addr = val << 8;

	for(int i = 0; i < 256; i++)
	{
		ppu->oam_data[ppu->oam_address] = xnes_cpu_read8(&ppu->ctx->cpu, addr);
		ppu->oam_address++;
		addr++;
	}
	ppu->ctx->cpu.stall += 513;
	if(ppu->ctx->cpu.cycles & 0x1)
		ppu->ctx->cpu.stall++;
}

static void ppu_increment_x(struct xnes_ppu_t * ppu)
{
	if((ppu->v & 0x001F) == 31)
	{
		ppu->v &= 0xFFE0;
		ppu->v ^= 0x0400;
	}
	else
		ppu->v++;
}

static void ppu_increment_y(struct xnes_ppu_t * ppu)
{
	if((ppu->v & 0x7000) != 0x7000)
		ppu->v += 0x1000;
	else
	{
		ppu->v &= 0x8FFF;
		uint16_t y = (ppu->v & 0x03E0) >> 5;
		if(y == 29)
		{
			y = 0;
			ppu->v ^= 0x0800;
		}
		else if(y == 31)
			y = 0;
		else
			y++;
		ppu->v = (ppu->v & 0xFC1F) | (y << 5);
	}
}

static void ppu_copy_x(struct xnes_ppu_t * ppu)
{
	ppu->v = (ppu->v & 0xFBE0) | (ppu->t & 0x041F);
}

static void ppu_copy_y(struct xnes_ppu_t * ppu)
{
	ppu->v = (ppu->v & 0x841F) | (ppu->t & 0x7BE0);
}

static void ppu_set_vertical_blank(struct xnes_ppu_t * ppu)
{
	uint32_t * p = ppu->front;
	ppu->front = ppu->back;
	ppu->back = p;
	ppu->nmi_occurred = 1;
	ppu_nmi_change(ppu);
}

static void ppu_clear_vertical_blank(struct xnes_ppu_t * ppu)
{
	ppu->nmi_occurred = 0;
	ppu_nmi_change(ppu);
}

static void ppu_fetch_name_table_byte(struct xnes_ppu_t * ppu)
{
	uint16_t v = ppu->v;
	uint16_t addr = 0x2000 | (v & 0x0FFF);
	ppu->name_table_byte = ppu_read8(ppu, addr);
}

static void ppu_fetch_attribute_table_byte(struct xnes_ppu_t * ppu)
{
	uint16_t v = ppu->v;
	uint16_t addr = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
	uint16_t shift = ((v >> 4) & 4) | (v & 2);
	ppu->attributeTableByte = ((ppu_read8(ppu, addr) >> shift) & 3) << 2;
}

static void ppu_fetch_low_tile_byte(struct xnes_ppu_t * ppu)
{
	uint16_t fine_y = (ppu->v >> 12) & 7;
	uint8_t table = ppu->flag_background_table;
	uint8_t tile = ppu->name_table_byte;
	uint16_t addr = 0x1000 * (uint16_t)table + (uint16_t)tile * 16 + fine_y;
	ppu->low_tile_byte = ppu_read8(ppu, addr);
}

static void ppu_fetch_high_tile_byte(struct xnes_ppu_t * ppu)
{
	uint16_t fine_y = (ppu->v >> 12) & 7;
	uint8_t table = ppu->flag_background_table;
	uint8_t tile = ppu->name_table_byte;
	uint16_t addr = 0x1000 * (uint16_t)table + (uint16_t)tile * 16 + fine_y;
	ppu->high_tile_byte = ppu_read8(ppu, addr + 8);
}

static void ppu_store_tile_data(struct xnes_ppu_t * ppu)
{
	uint32_t data = 0;
	for(int i = 0; i < 8; i++)
	{
		uint8_t a = ppu->attributeTableByte;
		uint8_t p1 = (ppu->low_tile_byte & 0x80) >> 7;
		uint8_t p2 = (ppu->high_tile_byte & 0x80) >> 6;
		ppu->low_tile_byte <<= 1;
		ppu->high_tile_byte <<= 1;
		data <<= 4;
		data |= (uint32_t)(a | p1 | p2);
	}
	ppu->tile_data |= (uint64_t)data;
}

static uint32_t ppu_fetch_tile_data(struct xnes_ppu_t * ppu)
{
	return (uint32_t)(ppu->tile_data >> 32);
}

static uint8_t ppu_background_pixel(struct xnes_ppu_t * ppu)
{
	if(ppu->flag_show_background == 0)
		return 0;
	uint32_t data = ppu_fetch_tile_data(ppu) >> ((7 - ppu->x) * 4);
	return (uint8_t)(data & 0x0F);
}

static void ppu_sprite_pixel(struct xnes_ppu_t * ppu, uint8_t * a, uint8_t * b)
{
	if(ppu->flag_show_sprites == 0)
	{
		*a = 0;
		*b = 0;
		return;
	}
	for(int i = 0; i < ppu->sprite_count; i++)
	{
		int offset = (ppu->cycles - 1) - (int)(ppu->sprite_positions[i]);
		if(offset < 0 || offset > 7)
			continue;
		offset = 7 - offset;
		uint8_t color = (uint8_t)((ppu->sprite_patterns[i] >> (uint8_t)(offset * 4)) & 0x0F);
		if(color % 4 == 0)
			continue;
		*a = (uint8_t)i;
		*b = color;
		return;
	}
	*a = 0;
	*b = 0;
}

static void ppu_render_pixel(struct xnes_ppu_t * ppu)
{
	int x = ppu->cycles - 1;
	int y = ppu->scanline;
	uint8_t background = ppu_background_pixel(ppu);
	uint8_t i, sprite;
	ppu_sprite_pixel(ppu, &i, &sprite);
	if((x < 8) && (ppu->flag_show_left_background == 0))
		background = 0;
	if((x < 8) && (ppu->flag_show_left_sprites == 0))
		sprite = 0;
	uint8_t b = ((background % 4) != 0);
	uint8_t s = ((sprite % 4) != 0);
	uint8_t color;
	if(!b && !s)
		color = 0;
	else if(!b && s)
		color = sprite | 0x10;
	else if(b && !s)
		color = background;
	else
	{
		if(ppu->sprite_indexes[i] == 0 && x < 255)
			ppu->flag_sprite_zero_hit = 1;
		if(ppu->sprite_priorities[i] == 0)
			color = sprite | 0x10;
		else
			color = background;
	}
	uint32_t c = ppu->palette[ppu_read_palette(ppu, (uint16_t)color) % 64];
	ppu->back[256 * y + x] = c;
}

static uint32_t ppu_fetch_sprite_pattern(struct xnes_ppu_t * ppu, int i, int row)
{
	uint8_t tile = ppu->oam_data[i * 4 + 1];
	uint8_t attributes = ppu->oam_data[i * 4 + 2];
	uint16_t addr;
	if(ppu->flag_sprite_size == 0)
	{
		if((attributes & 0x80) == 0x80)
			row = 7 - row;
		uint8_t table = ppu->flag_sprite_table;
		addr = 0x1000 * (uint16_t)(table) + (uint16_t)(tile) * 16 + (uint16_t)(row);
	}
	else
	{
		if((attributes & 0x80) == 0x80)
			row = 15 - row;
		uint8_t table = tile & 1;
		tile &= 0xFE;
		if(row > 7)
		{
			tile++;
			row -= 8;
		}
		addr = 0x1000 * (uint16_t)(table) + (uint16_t)(tile) * 16 + (uint16_t)(row);
	}
	uint8_t a = (attributes & 3) << 2;
	uint8_t low_tile_byte = ppu_read8(ppu, addr);
	uint8_t high_tile_byte = ppu_read8(ppu, addr + 8);
	uint32_t data = 0;
	for(int i = 0; i < 8; i++)
	{
		uint32_t p1, p2;
		if((attributes & 0x40) == 0x40)
		{
			p1 = (low_tile_byte & 1) << 0;
			p2 = (high_tile_byte & 1) << 1;
			low_tile_byte >>= 1;
			high_tile_byte >>= 1;
		}
		else
		{
			p1 = (low_tile_byte & 0x80) >> 7;
			p2 = (high_tile_byte & 0x80) >> 6;
			low_tile_byte <<= 1;
			high_tile_byte <<= 1;
		}
		data <<= 4;
		data |= (uint32_t)(a | p1 | p2);
	}
	return data;
}

static void ppu_evaluate_sprites(struct xnes_ppu_t * ppu)
{
	int h;
	if(ppu->flag_sprite_size == 0)
		h = 8;
	else
		h = 16;
	int count = 0;
	for(int i = 0; i < 64; i++)
	{
		uint8_t y = ppu->oam_data[i * 4 + 0];
		uint8_t a = ppu->oam_data[i * 4 + 2];
		uint8_t x = ppu->oam_data[i * 4 + 3];
		int row = ppu->scanline - (int)(y);
		if(row < 0 || row >= h)
			continue;
		if(count < 8)
		{
			ppu->sprite_patterns[count] = ppu_fetch_sprite_pattern(ppu, i, row);
			ppu->sprite_positions[count] = x;
			ppu->sprite_priorities[count] = (a >> 5) & 1;
			ppu->sprite_indexes[count] = (uint8_t)(i);
		}
		count++;
	}
	if(count > 8)
	{
		count = 8;
		ppu->flag_sprite_overflow = 1;
	}
	ppu->sprite_count = count;
}

static void ppu_tick(struct xnes_ppu_t * ppu)
{
	if(ppu->nmi_delay > 0)
	{
		ppu->nmi_delay--;
		if((ppu->nmi_delay == 0) && ppu->nmi_output && ppu->nmi_occurred)
			xnes_cpu_trigger_nmi(&ppu->ctx->cpu);
	}

	if(ppu->flag_show_background != 0 || ppu->flag_show_sprites != 0)
	{
		if(ppu->f == 1 && ppu->scanline == 261 && ppu->cycles == 339)
		{
			ppu->cycles = 0;
			ppu->scanline = 0;
			ppu->frame++;
			ppu->f ^= 1;
			return;
		}
	}
	ppu->cycles++;
	if(ppu->cycles > 340)
	{
		ppu->cycles = 0;
		ppu->scanline++;
		if(ppu->scanline > 261)
		{
			ppu->scanline = 0;
			ppu->frame++;
			ppu->f ^= 1;
		}
	}
}

static void ppu_step(struct xnes_ppu_t * ppu)
{
	ppu_tick(ppu);

	uint8_t rendering_enabled = (ppu->flag_show_background != 0 || ppu->flag_show_sprites != 0);
	uint8_t pre_line = (ppu->scanline == 261);
	uint8_t visible_line = (ppu->scanline < 240);
	uint8_t render_line = (pre_line || visible_line);
	uint8_t pre_fetch_cycle = (ppu->cycles >= 321 && ppu->cycles <= 336);
	uint8_t visible_cycle = (ppu->cycles >= 1 && ppu->cycles <= 256);
	uint8_t fetch_cycle = (pre_fetch_cycle || visible_cycle);

	if(rendering_enabled)
	{
		if(visible_line && visible_cycle)
			ppu_render_pixel(ppu);
		if(render_line && fetch_cycle)
		{
			ppu->tile_data <<= 4;
			switch(ppu->cycles % 8)
			{
			case 1:
				ppu_fetch_name_table_byte(ppu);
				break;
			case 3:
				ppu_fetch_attribute_table_byte(ppu);
				break;
			case 5:
				ppu_fetch_low_tile_byte(ppu);
				break;
			case 7:
				ppu_fetch_high_tile_byte(ppu);
				break;
			case 0:
				ppu_store_tile_data(ppu);
				break;
			default:
				break;
			}
		}
		if(pre_line && ppu->cycles >= 280 && ppu->cycles <= 304)
			ppu_copy_y(ppu);
		if(render_line)
		{
			if(fetch_cycle && ((ppu->cycles % 8) == 0))
				ppu_increment_x(ppu);
			if(ppu->cycles == 256)
				ppu_increment_y(ppu);
			if(ppu->cycles == 257)
				ppu_copy_x(ppu);
		}
	}

	if(rendering_enabled)
	{
		if(ppu->cycles == 257)
		{
			if(visible_line)
				ppu_evaluate_sprites(ppu);
			else
				ppu->sprite_count = 0;
		}
	}

	if(ppu->scanline == 241 && ppu->cycles == 1)
		ppu_set_vertical_blank(ppu);
	if(pre_line && ppu->cycles == 1)
	{
		ppu_clear_vertical_blank(ppu);
		ppu->flag_sprite_zero_hit = 0;
		ppu->flag_sprite_overflow = 0;
	}
}

void xnes_ppu_init(struct xnes_ppu_t * ppu, struct xnes_ctx_t * ctx)
{
	ppu->ctx = ctx;
	ppu->front = &ppu->front_buf[0];
	ppu->back = &ppu->back_buf[0];
	xnes_ppu_reset(ppu);
}

static const uint32_t default_palette[64] = {
    0xff666666, 0xff002a88, 0xff1412a7, 0xff3b00a4, 0xff5c007e, 0xff6e0040, 0xff6c0600, 0xff561d00,
    0xff333500, 0xff0b4800, 0xff005200, 0xff004f08, 0xff00404d, 0xff000000, 0xff000000, 0xff000000,
    0xffadadad, 0xff155fd9, 0xff4240ff, 0xff7527fe, 0xffa01acc, 0xffb71e7b, 0xffb53120, 0xff994e00,
    0xff6b6d00, 0xff388700, 0xff0c9300, 0xff008f32, 0xff007c8d, 0xff000000, 0xff000000, 0xff000000,
    0xfffffeff, 0xff64b0ff, 0xff9290ff, 0xffc676ff, 0xfff36aff, 0xfffe6ecc, 0xfffe8170, 0xffea9e22,
    0xffbcbe00, 0xff88d800, 0xff5ce430, 0xff45e082, 0xff48cdde, 0xff4f4f4f, 0xff000000, 0xff000000,
    0xfffffeff, 0xffc0dfff, 0xffd3d2ff, 0xffe8c8ff, 0xfffbc2ff, 0xfffec4ea, 0xfffeccc5, 0xfff7d8a5,
    0xffe4e594, 0xffcfef96, 0xffbdf4ab, 0xffb3f3cc, 0xffb5ebf2, 0xffb8b8b8, 0xff000000, 0xff000000,
};

void xnes_ppu_reset(struct xnes_ppu_t * ppu)
{
	xnes_ppu_set_palette(ppu, (uint32_t *)&default_palette[0]);
	ppu->cycles = 340;
	ppu->scanline = 240;
	ppu->frame = 0;
	ppu_write_control(ppu, 0);
	ppu_write_mask(ppu, 0);
	ppu_write_oam_address(ppu, 0);
}

void xnes_ppu_step(struct xnes_ppu_t * ppu)
{
	ppu_step(ppu);
	xnes_cartridge_mapper_ppu_step(ppu->ctx);
}

uint8_t xnes_ppu_read_register(struct xnes_ppu_t * ppu, uint16_t addr)
{
	switch(addr)
	{
	case 0x2000: /* PPU_CTRL - write */
		break;
	case 0x2001: /* PPU_MASK - write */
		break;
	case 0x2002: /* PPU_STATUS - read */
		return ppu_read_status(ppu);
	case 0x2003: /* OAM_ADDR - write */
		break;
	case 0x2004: /* OAM_DATA - read/write */
		return ppu_read_oam_data(ppu);
	case 0x2005: /* PPU_SCROLL - write 2x */
		break;
	case 0x2006: /* PPU_ADDR - write 2x */
		break;
	case 0x2007: /* PPU_DATA - read/write */
		return ppu_read_data(ppu);
	case 0x4014: /* OAM_DMA - write */
		break;
	default:
		break;
	}
	return 0;
}

void xnes_ppu_write_register(struct xnes_ppu_t * ppu, uint16_t addr, uint8_t val)
{
	ppu->reg = val;
	switch(addr)
	{
	case 0x2000: /* PPU_CTRL - write */
		ppu_write_control(ppu, val);
		break;
	case 0x2001: /* PPU_MASK - write */
		ppu_write_mask(ppu, val);
		break;
	case 0x2002: /* PPU_STATUS - read */
		break;
	case 0x2003: /* OAM_ADDR - write */
		ppu_write_oam_address(ppu, val);
		break;
	case 0x2004: /* OAM_DATA - read/write */
		ppu_write_oam_data(ppu, val);
		break;
	case 0x2005: /* PPU_SCROLL - write 2x */
		ppu_write_scroll(ppu, val);
		break;
	case 0x2006: /* PPU_ADDR - write 2x */
		ppu_write_address(ppu, val);
		break;
	case 0x2007: /* PPU_DATA - read/write */
		ppu_write_data(ppu, val);
		break;
	case 0x4014: /* OAM_DMA - write */
		ppu_write_dma(ppu, val);
		break;
	default:
		break;
	}
}

void xnes_ppu_set_palette(struct xnes_ppu_t * ppu, uint32_t * palette)
{
	if(ppu && palette)
		xnes_memcpy(&ppu->palette[0], palette, sizeof(uint32_t) * 64);
}

uint8_t xnes_ppu_is_white_pixel(struct xnes_ppu_t * ppu, uint8_t x, uint8_t y)
{
	uint32_t c = ppu->front[(y * 256) + x];
	if(((c & 0x00ff0000) > 0x00f00000) && ((c & 0x0000ff00) > 0x0000f000) && ((c & 0x000000ff) > 0x000000f0))
		return 1;
	return 0;
}
