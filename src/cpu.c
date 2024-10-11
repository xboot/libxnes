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

#include <ppu.h>
#include <apu.h>
#include <controller.h>
#include <cartridge.h>
#include <cpu.h>

enum {
    CPU_INTERRUPT_NMI				= (1 << 0),
    CPU_INTERRUPT_IRQ				= (1 << 1),
};

enum {
	CPU_ADDR_MODE_NONE				= 0,
	CPU_ADDR_MODE_ABSOLUTE			= 1,
	CPU_ADDR_MODE_ABSOLUTE_X		= 2,
	CPU_ADDR_MODE_ABSOLUTE_Y		= 3,
	CPU_ADDR_MODE_ACCUMULATOR		= 4,
	CPU_ADDR_MODE_IMMEDIATE			= 5,
	CPU_ADDR_MODE_IMPLIED			= 6,
	CPU_ADDR_MODE_INDIRECT_X		= 7,
	CPU_ADDR_MODE_INDIRECT			= 8,
	CPU_ADDR_MODE_INDIRECT_Y		= 9,
	CPU_ADDR_MODE_RELATIVE			= 10,
	CPU_ADDR_MODE_ZEROPAGE			= 11,
	CPU_ADDR_MODE_ZEROPAGE_X		= 12,
	CPU_ADDR_MODE_ZEROPAGE_Y		= 13,
};

static const uint8_t cpu_instruction_mode[256] = {
	      /* 00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F */
	/* 00 */  6,  7,  6,  7, 11, 11, 11, 11,  6,  5,  4,  5,  1,  1,  1,  1,
	/* 10 */ 10,  9,  6,  9, 12, 12, 12, 12,  6,  3,  6,  3,  2,  2,  2,  2,
	/* 20 */  1,  7,  6,  7, 11, 11, 11, 11,  6,  5,  4,  5,  1,  1,  1,  1,
	/* 30 */ 10,  9,  6,  9, 12, 12, 12, 12,  6,  3,  6,  3,  2,  2,  2,  2,
	/* 40 */  6,  7,  6,  7, 11, 11, 11, 11,  6,  5,  4,  5,  1,  1,  1,  1,
	/* 50 */ 10,  9,  6,  9, 12, 12, 12, 12,  6,  3,  6,  3,  2,  2,  2,  2,
	/* 60 */  6,  7,  6,  7, 11, 11, 11, 11,  6,  5,  4,  5,  8,  1,  1,  1,
	/* 70 */ 10,  9,  6,  9, 12, 12, 12, 12,  6,  3,  6,  3,  2,  2,  2,  2,
	/* 80 */  5,  7,  5,  7, 11, 11, 11, 11,  6,  5,  6,  5,  1,  1,  1,  1,
	/* 90 */ 10,  9,  6,  9, 12, 12, 13, 13,  6,  3,  6,  3,  2,  2,  3,  3,
	/* A0 */  5,  7,  5,  7, 11, 11, 11, 11,  6,  5,  6,  5,  1,  1,  1,  1,
	/* B0 */ 10,  9,  6,  9, 12, 12, 13, 13,  6,  3,  6,  3,  2,  2,  3,  3,
	/* C0 */  5,  7,  5,  7, 11, 11, 11, 11,  6,  5,  6,  5,  1,  1,  1,  1,
	/* D0 */ 10,  9,  6,  9, 12, 12, 12, 12,  6,  3,  6,  3,  2,  2,  2,  2,
	/* E0 */  5,  7,  5,  7, 11, 11, 11, 11,  6,  5,  6,  5,  1,  1,  1,  1,
	/* F0 */ 10,  9,  6,  9, 12, 12, 12, 12,  6,  3,  6,  3,  2,  2,  2,  2,
};

static const uint8_t cpu_instruction_size[256] = {
	      /* 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F */
	/* 00 */ 1, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* 10 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* 20 */ 3, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* 30 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* 40 */ 1, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* 50 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* 60 */ 1, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* 70 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* 80 */ 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* 90 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* A0 */ 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* B0 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* C0 */ 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* D0 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* E0 */ 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* F0 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
};

static const uint8_t cpu_instruction_cycle[256] = {
	      /* 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F */
	/* 00 */ 7, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
	/* 10 */ 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	/* 20 */ 6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
	/* 30 */ 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	/* 40 */ 6, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
	/* 50 */ 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	/* 60 */ 6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
	/* 70 */ 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	/* 80 */ 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	/* 90 */ 2, 6, 0, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
	/* A0 */ 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	/* B0 */ 2, 5, 0, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
	/* C0 */ 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	/* D0 */ 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	/* E0 */ 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	/* F0 */ 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
};

static const uint8_t cpu_instruction_page_cycle[256] = {
	      /* 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F */
	/* 00 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 10 */ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	/* 20 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 30 */ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	/* 40 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 50 */ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	/* 60 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 70 */ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	/* 80 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 90 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* A0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* B0 */ 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
	/* C0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* D0 */ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	/* E0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* F0 */ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
};

uint8_t xnes_cpu_read8(struct xnes_cpu_t * cpu, uint16_t addr)
{
	struct xnes_ctx_t * ctx = cpu->ctx;

	switch(addr >> 13)
	{
	/* [0x0000, 0x1FFF] - RAM */
	case 0:
		return cpu->ram[addr & 0x7ff];

	/* [0x2000, 0x3FFF] - PPU */
	case 1:
		return xnes_ppu_read_register(&ctx->ppu, 0x2000 | (addr & 0x7));

	/* [0x4000, 0x5FFF] - APU, DMC, Joystick, Zapper, Cartridge (maybe mapper registers) */
	/* [0x6000, 0x7FFF] - Cartridge RAM (maybe battery-backed) */
	/* [0x8000, 0x9FFF] - PRG ROM */
	/* [0xA000, 0xBFFF] - PRG ROM */
	/* [0xC000, 0xDFFF] - PRG ROM */
	/* [0xE000, 0xFFFF] - PRG ROM, NMI:0xFFFA, RST:0xFFFC, IRQ/BRK: 0XFFFE */
	default:
		switch(addr)
		{
		case 0x4000 ... 0x4013: /* APU */
			return xnes_apu_read_register(&ctx->apu, addr);
		case 0x4014: /* OAMDMA */
			return xnes_ppu_read_register(&ctx->ppu, addr);
		case 0x4015: /* APU */
			return xnes_apu_read_register(&ctx->apu, addr);
		case 0x4016 ... 0x4017: /* CONTROLLER */
			return xnes_controller_read_register(&ctx->ctl, addr);
		case 0x4018 ... 0x401f: /* UNUSED */
			break;
		default: /* CARTRIDGE */
			return xnes_cartridge_mapper_cpu_read(ctx, addr);
		}
		break;
	}
	return 0;
}

void xnes_cpu_write8(struct xnes_cpu_t * cpu, uint16_t addr, uint8_t val)
{
	struct xnes_ctx_t * ctx = cpu->ctx;

	switch(addr >> 13)
	{
	/* [0x0000, 0x1FFF] - RAM */
	case 0:
		cpu->ram[addr & 0x7ff] = val;
		break;

	/* [0x2000, 0x3FFF] - PPU */
	case 1:
		xnes_ppu_write_register(&ctx->ppu, 0x2000 | (addr & 0x7), val);
		break;

	/* [0x4000, 0x5FFF] - APU, DMC, Joystick, Zapper, Cartridge (maybe mapper registers) */
	/* [0x6000, 0x7FFF] - Cartridge RAM (maybe battery-backed) */
	/* [0x8000, 0x9FFF] - PRG ROM */
	/* [0xA000, 0xBFFF] - PRG ROM */
	/* [0xC000, 0xDFFF] - PRG ROM */
	/* [0xE000, 0xFFFF] - PRG ROM, NMI:0xFFFA, RST:0xFFFC, IRQ/BRK: 0XFFFE */
	default:
		switch(addr)
		{
		case 0x4000 ... 0x4013: /* APU */
			xnes_apu_write_register(&ctx->apu, addr, val);
			break;
		case 0x4014: /* OAMDMA */
			xnes_ppu_write_register(&ctx->ppu, addr, val);
			break;
		case 0x4015: /* APU */
			xnes_apu_write_register(&ctx->apu, addr, val);
			break;
		case 0x4016: /* CONTROLLER */
			xnes_controller_write_register(&ctx->ctl, addr, val);
			break;
		case 0x4017: /* APU */
			xnes_apu_write_register(&ctx->apu, addr, val);
			break;
		case 0x4018 ... 0x401f: /* UNUSED */
			break;
		default: /* CARTRIDGE */
			xnes_cartridge_mapper_cpu_write(ctx, addr, val);
			break;
		}
		break;
	}
}

static inline uint16_t cpu_read16(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t lo = xnes_cpu_read8(cpu, addr);
	uint8_t hi = xnes_cpu_read8(cpu, addr + 1);
	return ((uint16_t)hi << 8) | lo;
}

static inline uint16_t cpu_read16_indirect_bug(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t lo = xnes_cpu_read8(cpu, addr);
	uint8_t hi = xnes_cpu_read8(cpu, (addr & 0xff00) | ((addr + 1) & 0x00ff));
	return ((uint16_t)hi << 8) | lo;
}

/*
 * Push a byte onto the stack
 */
static inline void cpu_stack_push8(struct xnes_cpu_t * cpu, uint8_t val)
{
	xnes_cpu_write8(cpu, 0x100 | cpu->sp, val);
	cpu->sp--;
}

/*
 * Pop a byte from the stack
 */
static inline uint8_t cpu_stack_pop8(struct xnes_cpu_t * cpu)
{
	cpu->sp++;
	return xnes_cpu_read8(cpu, 0x100 | cpu->sp);
}

/*
 * Push two bytes onto the stack
 */
static inline void cpu_stack_push16(struct xnes_cpu_t * cpu, uint16_t val)
{
	cpu_stack_push8(cpu, (val >> 8) & 0xff);
	cpu_stack_push8(cpu, (val >> 0) & 0xff);
}

/*
 * Pop two bytes from the stack
 */
static inline uint16_t cpu_stack_pop16(struct xnes_cpu_t * cpu)
{
	uint8_t lo = cpu_stack_pop8(cpu);
	uint8_t hi = cpu_stack_pop8(cpu);
	return ((uint16_t)hi << 8) | lo;
}

/*
 * Get the processor status flags
 */
static inline uint8_t cpu_get_flags(struct xnes_cpu_t * cpu)
{
	uint8_t flags = 0;

	flags |= cpu->c << 0;
	flags |= cpu->z << 1;
	flags |= cpu->i << 2;
	flags |= cpu->d << 3;
	flags |= cpu->b << 4;
	flags |= cpu->u << 5;
	flags |= cpu->v << 6;
	flags |= cpu->n << 7;

	return flags;
}

/*
 * Set the processor status flags
 */
static inline void cpu_set_flags(struct xnes_cpu_t * cpu, uint8_t flags)
{
	cpu->c = (flags >> 0) & 0x1;
	cpu->z = (flags >> 1) & 0x1;
	cpu->i = (flags >> 2) & 0x1;
	cpu->d = (flags >> 3) & 0x1;
	cpu->b = (flags >> 4) & 0x1;
	cpu->u = (flags >> 5) & 0x1;
	cpu->v = (flags >> 6) & 0x1;
	cpu->n = (flags >> 7) & 0x1;
}

/*
 * Set the zero flag if the argument is zero
 */
static inline void cpu_set_z(struct xnes_cpu_t * cpu, uint8_t val)
{
	if(val == 0)
		cpu->z = 1;
	else
		cpu->z = 0;
}

/*
 * Set the negative flag if the argument is negative (high bit is set)
 */
static inline void cpu_set_n(struct xnes_cpu_t * cpu, uint8_t val)
{
	if((val & 0x80) != 0)
		cpu->n = 1;
	else
		cpu->n = 0;
}

/*
 * Set the zero flag and the negative flag
 */
static inline void cpu_set_zn(struct xnes_cpu_t * cpu, uint8_t val)
{
	cpu_set_z(cpu, val);
	cpu_set_n(cpu, val);
}

static inline int cpu_add_branch_cycles(struct xnes_cpu_t * cpu, uint16_t addr)
{
	return ((cpu->pc & 0xff00) != (addr & 0xff00)) ? 2 : 1;
}

static inline void cpu_compare(struct xnes_cpu_t * cpu, uint8_t a, uint8_t b)
{
	cpu_set_zn(cpu, a - b);
	if(a >= b)
		cpu->c = 1;
	else
		cpu->c = 0;
}

/*
 * PHP - Push Processor Status
 */
static inline void cpu_php(struct xnes_cpu_t * cpu)
{
	cpu_stack_push8(cpu, cpu_get_flags(cpu) | 0x10);
}

/*
 * SEI - Set Interrupt Disable
 */
static inline void cpu_sei(struct xnes_cpu_t * cpu)
{
	cpu->i = 1;
}

/*
 * SHX - Stores X AND at addr
 */
static inline void cpu_shx(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t v = ((addr >> 8) + 1) & cpu->x;
	xnes_cpu_write8(cpu, (v << 8) | (addr & 0xff), v);
}

/*
 * SHY - Stores Y AND at addr
 */
static inline void cpu_shy(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t v = ((addr >> 8) + 1) & cpu->y;
	xnes_cpu_write8(cpu, (v << 8) | (addr & 0xff), v);
}

/*
 * ADC - Add with Carry
 */
static inline void cpu_adc(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t a = cpu->a;
	uint8_t b = xnes_cpu_read8(cpu, addr);
	uint8_t c = cpu->c;
	int v = a + b + c;
	cpu->a = v;
	cpu_set_zn(cpu, cpu->a);
	if(v > 0xff)
		cpu->c = 1;
	else
		cpu->c = 0;
	if((((a ^ b) & 0x80) == 0) && (((a ^ cpu->a) & 0x80) != 0))
		cpu->v = 1;
	else
		cpu->v = 0;
}

/*
 * AHX - Stores A AND X AND
 */
static inline void cpu_ahx(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t val = cpu->a & cpu->x & ((addr >> 8) + 1);
	xnes_cpu_write8(cpu, addr, val);
}

/*
 * AND - Logical AND
 */
static inline void cpu_and(struct xnes_cpu_t * cpu, uint16_t addr)
{
	cpu->a = cpu->a & xnes_cpu_read8(cpu, addr);
	cpu_set_zn(cpu, cpu->a);
}

/*
 * ALR - AND And LSR
 */
static inline void cpu_alr(struct xnes_cpu_t * cpu, uint16_t addr)
{
	cpu->a = cpu->a & xnes_cpu_read8(cpu, addr);
	cpu->c = (cpu->a >> 0) & 0x1;
	cpu->a >>= 0x1;
	cpu_set_zn(cpu, cpu->a);
}

/*
 * ANC - And With Carry
 */
static inline void cpu_anc(struct xnes_cpu_t * cpu, uint16_t addr)
{
	cpu_and(cpu, addr);
	if(cpu->a & 0x80)
		cpu->c = 1;
	else
		cpu->c = 0;
}

/*
 * ARR - AND and ROR
 */
static inline void cpu_arr(struct xnes_cpu_t * cpu, uint16_t addr)
{
	cpu->a &= xnes_cpu_read8(cpu, addr);
	cpu->a = (cpu->a >> 1) | (cpu->c << 7);
	cpu_set_zn(cpu, cpu->a);
	cpu->c = (cpu->a >> 6) & 0x1;
	if(((cpu->a >> 5) ^ (cpu->a >> 6)) & 0x1)
		cpu->v = 1;
	else
		cpu->v = 0;
}

/*
 * ASL - Arithmetic Shift Left
 */
static inline void cpu_asl(struct xnes_cpu_t * cpu, uint16_t addr, uint8_t mode)
{
	if(mode == CPU_ADDR_MODE_ACCUMULATOR)
	{
		cpu->c = (cpu->a >> 7) & 0x1;
		cpu->a <<= 1;
		cpu_set_zn(cpu, cpu->a);
	}
	else
	{
		uint8_t val = xnes_cpu_read8(cpu, addr);
		cpu->c = (val >> 7) & 0x1;
		val <<= 1;
		xnes_cpu_write8(cpu, addr, val);
		cpu_set_zn(cpu, val);
	}
}

/*
 * AXS - CMP and DEX at once
 */
static inline void cpu_axs(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t a = cpu->a & cpu->x;
	uint8_t b = xnes_cpu_read8(cpu, addr);

	cpu->c = (a >= b) ? 1 : 0;
	cpu->z = (a == b) ? 1 : 0;
	cpu->x = a - b;
	cpu->n = (cpu->x & 0x80) ? 1 : 0;
}

/*
 * BCC - Branch if Carry Clear
 */
static inline int cpu_bcc(struct xnes_cpu_t * cpu, uint16_t addr)
{
	int cycles = 0;

	if(cpu->c == 0)
	{
		cycles += cpu_add_branch_cycles(cpu, addr);
		cpu->pc = addr;
	}
	return cycles;
}

/*
 * BCS - Branch if Carry Set
 */
static inline int cpu_bcs(struct xnes_cpu_t * cpu, uint16_t addr)
{
	int cycles = 0;

	if(cpu->c != 0)
	{
		cycles += cpu_add_branch_cycles(cpu, addr);
		cpu->pc = addr;
	}
	return cycles;
}

/*
 * BEQ - Branch if Equal
 */
static inline int cpu_beq(struct xnes_cpu_t * cpu, uint16_t addr)
{
	int cycles = 0;

	if(cpu->z != 0)
	{
		cycles += cpu_add_branch_cycles(cpu, addr);
		cpu->pc = addr;
	}
	return cycles;
}

/*
 * BIT - Bit Test
 */
static inline void cpu_bit(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t val = xnes_cpu_read8(cpu, addr);
	cpu->v = (val >> 6) & 0x1;
	cpu_set_z(cpu, val & cpu->a);
	cpu_set_n(cpu, val);
}

/*
 * BMI - Branch if Minus
 */
static inline int cpu_bmi(struct xnes_cpu_t * cpu, uint16_t addr)
{
	int cycles = 0;

	if(cpu->n != 0)
	{
		cycles += cpu_add_branch_cycles(cpu, addr);
		cpu->pc = addr;
	}
	return cycles;
}

/*
 * BNE - Branch if Not Equal
 */
static inline int cpu_bne(struct xnes_cpu_t * cpu, uint16_t addr)
{
	int cycles = 0;

	if(cpu->z == 0)
	{
		cycles += cpu_add_branch_cycles(cpu, addr);
		cpu->pc = addr;
	}
	return cycles;
}

/*
 * BPL - Branch if Positive
 */
static inline int cpu_bpl(struct xnes_cpu_t * cpu, uint16_t addr)
{
	int cycles = 0;

	if(cpu->n == 0)
	{
		cycles += cpu_add_branch_cycles(cpu, addr);
		cpu->pc = addr;
	}
	return cycles;
}

/*
 * BRK - Force Interrupt
 */
static inline void cpu_brk(struct xnes_cpu_t * cpu)
{
	cpu_stack_push16(cpu, cpu->pc);
	cpu_php(cpu);
	cpu_sei(cpu);
	cpu->pc = cpu_read16(cpu, 0xfffe);
}

/*
 * BVC - Branch if Overflow Clear
 */
static inline int cpu_bvc(struct xnes_cpu_t * cpu, uint16_t addr)
{
	int cycles = 0;

	if(cpu->v == 0)
	{
		cycles += cpu_add_branch_cycles(cpu, addr);
		cpu->pc = addr;
	}
	return cycles;
}

/*
 * BVS - Branch if Overflow Set
 */
static inline int cpu_bvs(struct xnes_cpu_t * cpu, uint16_t addr)
{
	int cycles = 0;

	if(cpu->v != 0)
	{
		cycles += cpu_add_branch_cycles(cpu, addr);
		cpu->pc = addr;
	}
	return cycles;
}

/*
 * CLC - Clear Carry Flag
 */
static inline void cpu_clc(struct xnes_cpu_t * cpu)
{
	cpu->c = 0;
}

/*
 * CLD - Clear Decimal Mode
 */
static inline void cpu_cld(struct xnes_cpu_t * cpu)
{
	cpu->d = 0;
}

/*
 * CLI - Clear Interrupt Disable
 */
static inline void cpu_cli(struct xnes_cpu_t * cpu)
{
	cpu->i = 0;
}

/*
 * CLV - Clear Overflow Flag
 */
static inline void cpu_clv(struct xnes_cpu_t * cpu)
{
	cpu->v = 0;
}

/*
 * CMP - Compare
 */
static inline void cpu_cmp(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t val = xnes_cpu_read8(cpu, addr);
	cpu_compare(cpu, cpu->a, val);
}

/*
 * CPX - Compare X Register
 */
static inline void cpu_cpx(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t val = xnes_cpu_read8(cpu, addr);
	cpu_compare(cpu, cpu->x, val);
}

/*
 * CPY - Compare Y Register
 */
static inline void cpu_cpy(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t val = xnes_cpu_read8(cpu, addr);
	cpu_compare(cpu, cpu->y, val);
}

/*
 * DEC - Decrement Memory
 */
static inline void cpu_dec(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t val = xnes_cpu_read8(cpu, addr) - 1;
	xnes_cpu_write8(cpu, addr, val);
	cpu_set_zn(cpu, val);
}

/*
 * DEX - Decrement X Register
 */
static inline void cpu_dex(struct xnes_cpu_t * cpu)
{
	cpu->x--;
	cpu_set_zn(cpu, cpu->x);
}

/*
 * DEY - Decrement Y Register
 */
static inline void cpu_dey(struct xnes_cpu_t * cpu)
{
	cpu->y--;
	cpu_set_zn(cpu, cpu->y);
}

/*
 * EOR - Exclusive OR
 */
static inline void cpu_eor(struct xnes_cpu_t * cpu, uint16_t addr)
{
	cpu->a = cpu->a ^ xnes_cpu_read8(cpu, addr);
	cpu_set_zn(cpu, cpu->a);
}

/*
 * INC - Increment Memory
 */
static inline void cpu_inc(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t val = xnes_cpu_read8(cpu, addr) + 1;
	xnes_cpu_write8(cpu, addr, val);
	cpu_set_zn(cpu, val);
}

/*
 * INX - Increment X Register
 */
static inline void cpu_inx(struct xnes_cpu_t * cpu)
{
	cpu->x++;
	cpu_set_zn(cpu, cpu->x);
}

/*
 * INY - Increment Y Register
 */
static inline void cpu_iny(struct xnes_cpu_t * cpu)
{
	cpu->y++;
	cpu_set_zn(cpu, cpu->y);
}

/*
 * JMP - Jump
 */
static inline void cpu_jmp(struct xnes_cpu_t * cpu, uint16_t addr)
{
	cpu->pc = addr;
}

/*
 * JSR - Jump to Subroutine
 */
static inline void cpu_jsr(struct xnes_cpu_t * cpu, uint16_t addr)
{
	cpu_stack_push16(cpu, cpu->pc - 1);
	cpu->pc = addr;
}

/*
 * LAS - LDA/TSX oper
 */
static inline void cpu_las(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t val = xnes_cpu_read8(cpu, addr) & cpu->sp;
	cpu->a = cpu->x = cpu->sp = val;
	cpu_set_zn(cpu, val);
}

/*
 * LDA - Load Accumulator
 */
static inline void cpu_lda(struct xnes_cpu_t * cpu, uint16_t addr)
{
	cpu->a = xnes_cpu_read8(cpu, addr);
	cpu_set_zn(cpu, cpu->a);
}

/*
 * LDX - Load X Register
 */
static inline void cpu_ldx(struct xnes_cpu_t * cpu, uint16_t addr)
{
	cpu->x = xnes_cpu_read8(cpu, addr);
	cpu_set_zn(cpu, cpu->x);
}

/*
 * LDY - Load Y Register
 */
static inline void cpu_ldy(struct xnes_cpu_t * cpu, uint16_t addr)
{
	cpu->y = xnes_cpu_read8(cpu, addr);
	cpu_set_zn(cpu, cpu->y);
}

/*
 * LSR - Logical Shift Right
 */
static inline void cpu_lsr(struct xnes_cpu_t * cpu, uint16_t addr, uint8_t mode)
{
	if(mode == CPU_ADDR_MODE_ACCUMULATOR)
	{
		cpu->c = cpu->a & 0x1;
		cpu->a >>= 0x1;
		cpu_set_zn(cpu, cpu->a);
	}
	else
	{
		uint8_t val = xnes_cpu_read8(cpu, addr);
		cpu->c = val & 0x1;
		val >>= 0x1;
		xnes_cpu_write8(cpu, addr, val);
		cpu_set_zn(cpu, val);
	}
}

/*
 * ORA - Logical Inclusive OR
 */
static inline void cpu_ora(struct xnes_cpu_t * cpu, uint16_t addr)
{
	cpu->a = cpu->a | xnes_cpu_read8(cpu, addr);
	cpu_set_zn(cpu, cpu->a);
}

/*
 * PHA - Push Accumulator
 */
static inline void cpu_pha(struct xnes_cpu_t * cpu)
{
	cpu_stack_push8(cpu, cpu->a);
}

/*
 * PLA - Pull Accumulator
 */
static inline void cpu_pla(struct xnes_cpu_t * cpu)
{
	cpu->a = cpu_stack_pop8(cpu);
	cpu_set_zn(cpu, cpu->a);
}

/*
 * PLP - Pull Processor Status
 */
static inline void cpu_plp(struct xnes_cpu_t * cpu)
{
	cpu_set_flags(cpu, (cpu_stack_pop8(cpu) & 0xef) | 0x20);
}

/*
 * ROL - Rotate Left
 */
static inline void cpu_rol(struct xnes_cpu_t * cpu, uint16_t addr, uint8_t mode)
{
	if(mode == CPU_ADDR_MODE_ACCUMULATOR)
	{
		uint8_t c = cpu->c;
		cpu->c = (cpu->a >> 7) & 0x1;
		cpu->a = (cpu->a << 1) | c;
		cpu_set_zn(cpu, cpu->a);
	}
	else
	{
		uint8_t c = cpu->c;
		uint8_t val = xnes_cpu_read8(cpu, addr);
		cpu->c = (val >> 7) & 0x1;
		val = (val << 1) | c;
		xnes_cpu_write8(cpu, addr, val);
		cpu_set_zn(cpu, val);
	}
}

/*
 * ROR - Rotate Right
 */
static inline void cpu_ror(struct xnes_cpu_t * cpu, uint16_t addr, uint8_t mode)
{
	if(mode == CPU_ADDR_MODE_ACCUMULATOR)
	{
		uint8_t c = cpu->c;
		cpu->c = cpu->a & 0x1;
		cpu->a = (cpu->a >> 1) | (c << 7);
		cpu_set_zn(cpu, cpu->a);
	}
	else
	{
		uint8_t c = cpu->c;
		uint8_t val = xnes_cpu_read8(cpu, addr);
		cpu->c = val & 0x1;
		val = (val >> 1) | (c << 7);
		xnes_cpu_write8(cpu, addr, val);
		cpu_set_zn(cpu, val);
	}
}

/*
 * RTI - Return from Interrupt
 */
static inline void cpu_rti(struct xnes_cpu_t * cpu)
{
	cpu_set_flags(cpu, (cpu_stack_pop8(cpu) & 0xef) | 0x20);
	cpu->pc = cpu_stack_pop16(cpu);
}

/*
 * RTS - Return from Subroutine
 */
static inline void cpu_rts(struct xnes_cpu_t * cpu)
{
	cpu->pc = cpu_stack_pop16(cpu) + 1;
}

/*
 * SAX - Store A & X
 */
static inline void cpu_sax(struct xnes_cpu_t * cpu, uint16_t addr)
{
	xnes_cpu_write8(cpu, addr, cpu->a & cpu->x);
}

/*
 * SBC - Subtract with Carry
 */
static inline void cpu_sbc(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t a = cpu->a;
	uint8_t b = xnes_cpu_read8(cpu, addr);
	uint8_t c = cpu->c;
	int v = a - b - (1 - c);
	cpu->a = v;
	cpu_set_zn(cpu, cpu->a);
	if(v >= 0)
		cpu->c = 1;
	else
		cpu->c = 0;
	if((((a ^ b) & 0x80) != 0) && (((a ^ cpu->a) & 0x80) != 0))
		cpu->v = 1;
	else
		cpu->v = 0;
}

/*
 * SEC - Set Carry Flag
 */
static inline void cpu_sec(struct xnes_cpu_t * cpu)
{
	cpu->c = 1;
}

/*
 * SED - Set Decimal Flag
 */
static inline void cpu_sed(struct xnes_cpu_t * cpu)
{
	cpu->d = 1;
}

/*
 * STA - Store Accumulator
 */
static inline void cpu_sta(struct xnes_cpu_t * cpu, uint16_t addr)
{
	xnes_cpu_write8(cpu, addr, cpu->a);
}

/*
 * STX - Store X Register
 */
static inline void cpu_stx(struct xnes_cpu_t * cpu, uint16_t addr)
{
	xnes_cpu_write8(cpu, addr, cpu->x);
}

/*
 * STY - Store Y Register
 */
static inline void cpu_sty(struct xnes_cpu_t * cpu, uint16_t addr)
{
	xnes_cpu_write8(cpu, addr, cpu->y);
}

/*
 * TAS - Puts A AND X in SP and stores A AND X AND at addr
 */
static inline void cpu_tas(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t val = cpu->a & cpu->x;
	cpu->sp = val;
	xnes_cpu_write8(cpu, addr, val & ((addr >> 8) + 1));
}

/*
 * TAX - Transfer Accumulator to X
 */
static inline void cpu_tax(struct xnes_cpu_t * cpu)
{
	cpu->x = cpu->a;
	cpu_set_zn(cpu, cpu->x);
}

/*
 * TAY - Transfer Accumulator to Y
 */
static inline void cpu_tay(struct xnes_cpu_t * cpu)
{
	cpu->y = cpu->a;
	cpu_set_zn(cpu, cpu->y);
}

/*
 * TSX - Transfer Stack Pointer to X
 */
static inline void cpu_tsx(struct xnes_cpu_t * cpu)
{
	cpu->x = cpu->sp;
	cpu_set_zn(cpu, cpu->x);
}

/*
 * TXA - Transfer X to Accumulator
 */
static inline void cpu_txa(struct xnes_cpu_t * cpu)
{
	cpu->a = cpu->x;
	cpu_set_zn(cpu, cpu->a);
}

/*
 * TXS - Transfer X to Stack Pointer
 */
static inline void cpu_txs(struct xnes_cpu_t * cpu)
{
	cpu->sp = cpu->x;
}

/*
 * TYA - Transfer Y to Accumulator
 */
static inline void cpu_tya(struct xnes_cpu_t * cpu)
{
	cpu->a = cpu->y;
	cpu_set_zn(cpu, cpu->a);
}

/*
 * XAA - OR X and AND
 */
static inline void cpu_xaa(struct xnes_cpu_t * cpu, uint16_t addr)
{
	cpu->a = (cpu->a | 0xee) & cpu->x & xnes_cpu_read8(cpu, addr);
}

void xnes_cpu_init(struct xnes_cpu_t * cpu, struct xnes_ctx_t * ctx)
{
	xnes_memset(cpu, 0, sizeof(struct xnes_cpu_t));
	cpu->ctx = ctx;
	cpu->debugger = NULL;
	xnes_cpu_reset(cpu);
}

void xnes_cpu_reset(struct xnes_cpu_t * cpu)
{
	xnes_memset(cpu->ram, 0, sizeof(cpu->ram));
	cpu->cycles = 0;
	cpu->stall = 0;
	cpu->pc = cpu_read16(cpu, 0xfffc);
	cpu->sp = 0xfd;
	cpu_set_flags(cpu, 0x24);
	cpu->interrupt = 0;
}

void xnes_cpu_trigger_nmi(struct xnes_cpu_t * cpu)
{
	cpu->interrupt |= CPU_INTERRUPT_NMI;
}

void xnes_cpu_trigger_irq(struct xnes_cpu_t * cpu)
{
	if(cpu->i == 0)
		cpu->interrupt |= CPU_INTERRUPT_IRQ;
}

int xnes_cpu_step(struct xnes_cpu_t * cpu)
{
	if(cpu->stall > 0)
	{
		cpu->stall--;
		return 1;
	}
	if(cpu->debugger && cpu->debugger(cpu->ctx))
		return 0;

	int cycles = 0;
	if(cpu->interrupt & CPU_INTERRUPT_NMI)
	{
		cpu_stack_push16(cpu, cpu->pc);
		cpu_php(cpu);
		cpu->pc = cpu_read16(cpu, 0xfffa);
		cpu->i = 1;
		cycles += 7;
		cpu->interrupt &= ~CPU_INTERRUPT_NMI;
	}
	else if(cpu->interrupt & CPU_INTERRUPT_IRQ)
	{
		cpu_stack_push16(cpu, cpu->pc);
		cpu_php(cpu);
		cpu->pc = cpu_read16(cpu, 0xfffe);
		cpu->i = 1;
		cycles += 7;
		cpu->interrupt &= ~CPU_INTERRUPT_IRQ;
	}
	uint8_t opcode = xnes_cpu_read8(cpu, cpu->pc);
	uint8_t mode = cpu_instruction_mode[opcode];
	uint16_t addr;
	uint16_t v16;
	uint8_t v8;

	switch(mode)
	{
	case CPU_ADDR_MODE_ABSOLUTE:
		addr = cpu_read16(cpu, cpu->pc + 1);
		break;
	case CPU_ADDR_MODE_ABSOLUTE_X:
		v16 = cpu_read16(cpu, cpu->pc + 1);
		addr = v16 + (uint16_t)cpu->x;
		if((v16 & 0xff00) != (addr & 0xff00))
		{
			cycles += cpu_instruction_page_cycle[opcode];
			cpu_read16(cpu, addr - 0x100);
		}
		break;
	case CPU_ADDR_MODE_ABSOLUTE_Y:
		v16 = cpu_read16(cpu, cpu->pc + 1);
		addr = v16 + (uint16_t)cpu->y;
		if((v16 & 0xff00) != (addr & 0xff00))
		{
			cycles += cpu_instruction_page_cycle[opcode];
			cpu_read16(cpu, addr - 0x100);
		}
		break;
	case CPU_ADDR_MODE_ACCUMULATOR:
		cpu_read16(cpu, cpu->pc + 1);
		addr = 0;
		break;
	case CPU_ADDR_MODE_IMMEDIATE:
		addr = cpu->pc + 1;
		break;
	case CPU_ADDR_MODE_IMPLIED:
		cpu_read16(cpu, cpu->pc + 1);
		addr = 0;
		break;
	case CPU_ADDR_MODE_INDIRECT_X:
		addr = cpu_read16_indirect_bug(cpu, ((uint16_t)xnes_cpu_read8(cpu, cpu->pc + 1) + cpu->x) & 0xff);
		break;
	case CPU_ADDR_MODE_INDIRECT:
		addr = cpu_read16_indirect_bug(cpu, cpu_read16(cpu, cpu->pc + 1));
		break;
	case CPU_ADDR_MODE_INDIRECT_Y:
		v16 = cpu_read16_indirect_bug(cpu, (uint16_t)xnes_cpu_read8(cpu, cpu->pc + 1));
		addr = v16 + (uint16_t)cpu->y;
		if((v16 & 0xff00) != (addr & 0xff00))
		{
			cycles += cpu_instruction_page_cycle[opcode];
			cpu_read16(cpu, addr - 0x100);
		}
		break;
	case CPU_ADDR_MODE_RELATIVE:
		v8 = xnes_cpu_read8(cpu, cpu->pc + 1);
		if(v8 < 0x80)
			addr = cpu->pc + 2 + v8;
		else
			addr = cpu->pc + 2 + v8 - 0x100;
		break;
	case CPU_ADDR_MODE_ZEROPAGE:
		addr = xnes_cpu_read8(cpu, cpu->pc + 1);
		break;
	case CPU_ADDR_MODE_ZEROPAGE_X:
		addr = (xnes_cpu_read8(cpu, cpu->pc + 1) + cpu->x) & 0xff;
		break;
	case CPU_ADDR_MODE_ZEROPAGE_Y:
		addr = (xnes_cpu_read8(cpu, cpu->pc + 1) + cpu->y) & 0xff;
		break;
	default:
		addr = 0;
		break;
	}
	cpu->pc += cpu_instruction_size[opcode];
	cycles += cpu_instruction_cycle[opcode];
	switch(opcode)
	{
	case 0x61: /* ADC */
	case 0x65: /* ADC */
	case 0x69: /* ADC */
	case 0x6d: /* ADC */
	case 0x71: /* ADC */
	case 0x75: /* ADC */
	case 0x79: /* ADC */
	case 0x7d: /* ADC */
		cpu_adc(cpu, addr);
		break;

	case 0x93: /* AHX */
	case 0x9f: /* AHX */
		cpu_ahx(cpu, addr);
		break;

	case 0x4b: /* ALR */
		cpu_alr(cpu, addr);
		break;

	case 0x0b: /* ANC */
	case 0x2b: /* ANC */
		cpu_anc(cpu, addr);
		break;

	case 0x21: /* AND */
	case 0x25: /* AND */
	case 0x29: /* AND */
	case 0x2d: /* AND */
	case 0x31: /* AND */
	case 0x35: /* AND */
	case 0x39: /* AND */
	case 0x3d: /* AND */
		cpu_and(cpu, addr);
		break;

	case 0x6b: /* ARR */
		cpu_arr(cpu, addr);
		break;

	case 0x06: /* ASL */
	case 0x0a: /* ASL */
	case 0x0e: /* ASL */
	case 0x16: /* ASL */
	case 0x1e: /* ASL */
		cpu_asl(cpu, addr, mode);
		break;

	case 0xcb: /* AXS */
		cpu_axs(cpu, addr);
		break;

	case 0x90: /* BCC */
		cycles += cpu_bcc(cpu, addr);
		break;

	case 0xb0: /* BCS */
		cycles += cpu_bcs(cpu, addr);
		break;

	case 0xf0: /* BEQ */
		cycles += cpu_beq(cpu, addr);
		break;

	case 0x24: /* BIT */
	case 0x2c: /* BIT */
		cpu_bit(cpu, addr);
		break;

	case 0x30: /* BMI */
		cycles += cpu_bmi(cpu, addr);
		break;

	case 0xd0: /* BNE */
		cycles += cpu_bne(cpu, addr);
		break;

	case 0x10: /* BPL */
		cycles += cpu_bpl(cpu, addr);
		break;

	case 0x00: /* BRK */
		cpu_brk(cpu);
		break;

	case 0x50: /* BVC */
		cycles += cpu_bvc(cpu, addr);
		break;

	case 0x70: /* BVS */
		cycles += cpu_bvs(cpu, addr);
		break;

	case 0x18: /* CLC */
		cpu_clc(cpu);
		break;

	case 0xd8: /* CLD */
		cpu_cld(cpu);
		break;

	case 0x58: /* CLI */
		cpu_cli(cpu);
		break;

	case 0xb8: /* CLV */
		cpu_clv(cpu);
		break;

	case 0xc1: /* CMP */
	case 0xc5: /* CMP */
	case 0xc9: /* CMP */
	case 0xcd: /* CMP */
	case 0xd1: /* CMP */
	case 0xd5: /* CMP */
	case 0xd9: /* CMP */
	case 0xdd: /* CMP */
		cpu_cmp(cpu, addr);
		break;

	case 0xe0: /* CPX */
	case 0xe4: /* CPX */
	case 0xec: /* CPX */
		cpu_cpx(cpu, addr);
		break;

	case 0xc0: /* CPY */
	case 0xc4: /* CPY */
	case 0xcc: /* CPY */
		cpu_cpy(cpu, addr);
		break;

	case 0xc3: /* DCP */
	case 0xc7: /* DCP */
	case 0xcf: /* DCP */
	case 0xd3: /* DCP */
	case 0xd7: /* DCP */
	case 0xdb: /* DCP */
	case 0xdf: /* DCP */
		cpu_dec(cpu, addr);
		cpu_cmp(cpu, addr);
		break;

	case 0xc6: /* DEC */
	case 0xce: /* DEC */
	case 0xd6: /* DEC */
	case 0xde: /* DEC */
		cpu_dec(cpu, addr);
		break;

	case 0xca: /* DEX */
		cpu_dex(cpu);
		break;

	case 0x88: /* DEY */
		cpu_dey(cpu);
		break;

	case 0x41: /* EOR */
	case 0x45: /* EOR */
	case 0x49: /* EOR */
	case 0x4d: /* EOR */
	case 0x51: /* EOR */
	case 0x55: /* EOR */
	case 0x59: /* EOR */
	case 0x5d: /* EOR */
		cpu_eor(cpu, addr);
		break;

	case 0xe6: /* INC */
	case 0xee: /* INC */
	case 0xf6: /* INC */
	case 0xfe: /* INC */
		cpu_inc(cpu, addr);
		break;

	case 0xe8: /* INX */
		cpu_inx(cpu);
		break;

	case 0xc8: /* INY */
		cpu_iny(cpu);
		break;

	case 0xe3: /* ISC */
	case 0xe7: /* ISC */
	case 0xef: /* ISC */
	case 0xf3: /* ISC */
	case 0xf7: /* ISC */
	case 0xfb: /* ISC */
	case 0xff: /* ISC */
		cpu_inc(cpu, addr);
		cpu_sbc(cpu, addr);
		break;

	case 0x4c: /* JMP */
	case 0x6c: /* JMP */
		cpu_jmp(cpu, addr);
		break;

	case 0x20: /* JSR */
		cpu_jsr(cpu, addr);
		break;

	case 0x02: /* KIL */
	case 0x12: /* KIL */
	case 0x22: /* KIL */
	case 0x32: /* KIL */
	case 0x42: /* KIL */
	case 0x52: /* KIL */
	case 0x62: /* KIL */
	case 0x72: /* KIL */
	case 0x92: /* KIL */
	case 0xb2: /* KIL */
	case 0xd2: /* KIL */
	case 0xf2: /* KIL */
		break;

	case 0xbb: /* LAS */
		cpu_las(cpu, addr);
		break;

	case 0xa3: /* LAX */
	case 0xa7: /* LAX */
	case 0xab: /* LAX */
	case 0xaf: /* LAX */
	case 0xb3: /* LAX */
	case 0xb7: /* LAX */
	case 0xbf: /* LAX */
		cpu_lda(cpu, addr);
		cpu_ldx(cpu, addr);
		break;

	case 0xa1: /* LDA */
	case 0xa5: /* LDA */
	case 0xa9: /* LDA */
	case 0xad: /* LDA */
	case 0xb1: /* LDA */
	case 0xb5: /* LDA */
	case 0xb9: /* LDA */
	case 0xbd: /* LDA */
		cpu_lda(cpu, addr);
		break;

	case 0xa2: /* LDX */
	case 0xa6: /* LDX */
	case 0xae: /* LDX */
	case 0xb6: /* LDX */
	case 0xbe: /* LDX */
		cpu_ldx(cpu, addr);
		break;

	case 0xa0: /* LDY */
	case 0xa4: /* LDY */
	case 0xac: /* LDY */
	case 0xb4: /* LDY */
	case 0xbc: /* LDY */
		cpu_ldy(cpu, addr);
		break;

	case 0x46: /* LSR */
	case 0x4a: /* LSR */
	case 0x4e: /* LSR */
	case 0x56: /* LSR */
	case 0x5e: /* LSR */
		cpu_lsr(cpu, addr, mode);
		break;

	case 0x04: /* NOP */
	case 0x0c: /* NOP */
	case 0x14: /* NOP */
	case 0x1a: /* NOP */
	case 0x1c: /* NOP */
	case 0x34: /* NOP */
	case 0x3a: /* NOP */
	case 0x3c: /* NOP */
	case 0x44: /* NOP */
	case 0x54: /* NOP */
	case 0x5a: /* NOP */
	case 0x5c: /* NOP */
	case 0x64: /* NOP */
	case 0x74: /* NOP */
	case 0x7a: /* NOP */
	case 0x7c: /* NOP */
	case 0x80: /* NOP */
	case 0x82: /* NOP */
	case 0x89: /* NOP */
	case 0xc2: /* NOP */
	case 0xd4: /* NOP */
	case 0xda: /* NOP */
	case 0xdc: /* NOP */
	case 0xe2: /* NOP */
	case 0xea: /* NOP */
	case 0xf4: /* NOP */
	case 0xfa: /* NOP */
	case 0xfc: /* NOP */
		break;

	case 0x01: /* ORA */
	case 0x05: /* ORA */
	case 0x09: /* ORA */
	case 0x0d: /* ORA */
	case 0x11: /* ORA */
	case 0x15: /* ORA */
	case 0x19: /* ORA */
	case 0x1d: /* ORA */
		cpu_ora(cpu, addr);
		break;

	case 0x48: /* PHA */
		cpu_pha(cpu);
		break;

	case 0x08: /* PHP */
		cpu_php(cpu);
		break;

	case 0x68: /* PLA */
		cpu_pla(cpu);
		break;

	case 0x28: /* PLP */
		cpu_plp(cpu);
		break;

	case 0x23: /* RLA */
	case 0x27: /* RLA */
	case 0x2f: /* RLA */
	case 0x33: /* RLA */
	case 0x37: /* RLA */
	case 0x3b: /* RLA */
	case 0x3f: /* RLA */
		cpu_rol(cpu, addr, mode);
		cpu_and(cpu, addr);
		break;

	case 0x26: /* ROL */
	case 0x2a: /* ROL */
	case 0x2e: /* ROL */
	case 0x36: /* ROL */
	case 0x3e: /* ROL */
		cpu_rol(cpu, addr, mode);
		break;

	case 0x66: /* ROR */
	case 0x6a: /* ROR */
	case 0x6e: /* ROR */
	case 0x76: /* ROR */
	case 0x7e: /* ROR */
		cpu_ror(cpu, addr, mode);
		break;

	case 0x63: /* RRA */
	case 0x67: /* RRA */
	case 0x6f: /* RRA */
	case 0x73: /* RRA */
	case 0x77: /* RRA */
	case 0x7b: /* RRA */
	case 0x7f: /* RRA */
		cpu_ror(cpu, addr, mode);
		cpu_adc(cpu, addr);
		break;

	case 0x40: /* RTI */
		cpu_rti(cpu);
		break;

	case 0x60: /* RTS */
		cpu_rts(cpu);
		break;

	case 0x83: /* SAX */
	case 0x87: /* SAX */
	case 0x8f: /* SAX */
	case 0x97: /* SAX */
		cpu_sax(cpu, addr);
		break;

	case 0xe1: /* SBC */
	case 0xe5: /* SBC */
	case 0xe9: /* SBC */
	case 0xeb: /* SBC */
	case 0xed: /* SBC */
	case 0xf1: /* SBC */
	case 0xf5: /* SBC */
	case 0xf9: /* SBC */
	case 0xfd: /* SBC */
		cpu_sbc(cpu, addr);
		break;

	case 0x38: /* SEC */
		cpu_sec(cpu);
		break;

	case 0xf8: /* SED */
		cpu_sed(cpu);
		break;

	case 0x78: /* SEI */
		cpu_sei(cpu);
		break;

	case 0x9e: /* SHX */
		cpu_shx(cpu, addr);
		break;

	case 0x9c: /* SHY */
		cpu_shy(cpu, addr);
		break;

	case 0x03: /* SLO */
	case 0x07: /* SLO */
	case 0x0f: /* SLO */
	case 0x13: /* SLO */
	case 0x17: /* SLO */
	case 0x1b: /* SLO */
	case 0x1f: /* SLO */
		cpu_asl(cpu, addr, mode);
		cpu_ora(cpu, addr);
		break;

	case 0x43: /* SRE */
	case 0x47: /* SRE */
	case 0x4f: /* SRE */
	case 0x53: /* SRE */
	case 0x57: /* SRE */
	case 0x5b: /* SRE */
	case 0x5f: /* SRE */
		cpu_lsr(cpu, addr, mode);
		cpu_eor(cpu, addr);
		break;

	case 0x81: /* STA */
	case 0x85: /* STA */
	case 0x8d: /* STA */
	case 0x91: /* STA */
	case 0x95: /* STA */
	case 0x99: /* STA */
	case 0x9d: /* STA */
		cpu_sta(cpu, addr);
		break;

	case 0x86: /* STX */
	case 0x8e: /* STX */
	case 0x96: /* STX */
		cpu_stx(cpu, addr);
		break;

	case 0x84: /* STY */
	case 0x8c: /* STY */
	case 0x94: /* STY */
		cpu_sty(cpu, addr);
		break;

	case 0x9b: /* TAS */
		cpu_tas(cpu, addr);
		break;

	case 0xaa: /* TAX */
		cpu_tax(cpu);
		break;

	case 0xa8: /* TAY */
		cpu_tay(cpu);
		break;

	case 0xba: /* TSX */
		cpu_tsx(cpu);
		break;

	case 0x8a: /* TXA */
		cpu_txa(cpu);
		break;

	case 0x9a: /* TXS */
		cpu_txs(cpu);
		break;

	case 0x98: /* TYA */
		cpu_tya(cpu);
		break;

	case 0x8b: /* XAA */
		cpu_xaa(cpu, addr);
		break;

	default:
		break;
	}

	cpu->cycles += cycles;
	return cycles;
}
