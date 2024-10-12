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
#include <apu.h>
#include <dma.h>

void xnes_dma_init(struct xnes_dma_t * dma, struct xnes_ctx_t * ctx)
{
	dma->ctx = ctx;
	xnes_dma_reset(dma);
}

void xnes_dma_reset(struct xnes_dma_t * dma)
{
}

static const uint8_t dmc_table[] = {
	214, 190, 170, 160, 143, 127, 113, 107, 95, 80, 71, 64, 53, 42, 36, 27,
};

static inline void dmc_write_control(struct xnes_apu_dmc_t * dmc, uint8_t value)
{
	dmc->irq = (value & 0x80) ? 1 : 0;
	dmc->loop = (value & 0x40) ? 1 : 0;
	dmc->tick_period = dmc_table[value & 0xf];
}

static inline void dmc_write_value(struct xnes_apu_dmc_t * dmc, uint8_t value)
{
	dmc->value = value & 0x7f;
}

static inline void dmc_write_address(struct xnes_apu_dmc_t * dmc, uint8_t value)
{
	dmc->sample_address = 0xc000 | (((uint16_t)value) << 6);
}

static inline void dmc_write_length(struct xnes_apu_dmc_t * dmc, uint8_t value)
{
	dmc->sample_length = (((uint16_t)value) << 4) | 0x1;
}

static inline void oam_write(struct xnes_dma_t * dma, uint8_t val)
{
	struct xnes_cpu_t * cpu = &dma->ctx->cpu;
	uint16_t addr = (uint16_t)val << 8;

	for(int i = 0; i < 256; i++)
	{
		uint8_t v = xnes_cpu_read8(cpu, addr++);
		xnes_cpu_write8(cpu, 0x2004, v);
	}
	cpu->stall += 513;
	if(cpu->cycles & 0x1)
		cpu->stall++;
}

uint8_t xnes_dma_read_register(struct xnes_dma_t * dma, uint16_t addr)
{
	switch(addr)
	{
	case 0x4010: /* DMC_FREQ */
		break;
	case 0x4011: /* DMC_RAW */
		break;
	case 0x4012: /* DMC_START */
		break;
	case 0x4013: /* DMC_LEN */
		break;
	case 0x4014: /* OAM_DMA */
		break;
	default:
		break;
	}
	return 0;
}

void xnes_dma_write_register(struct xnes_dma_t * dma, uint16_t addr, uint8_t val)
{
	switch(addr)
	{
	case 0x4010: /* DMC_FREQ */
		dmc_write_control(&dma->ctx->apu.dmc, val);
		break;
	case 0x4011: /* DMC_RAW */
		dmc_write_value(&dma->ctx->apu.dmc, val);
		break;
	case 0x4012: /* DMC_START */
		dmc_write_address(&dma->ctx->apu.dmc, val);
		break;
	case 0x4013: /* DMC_LEN */
		dmc_write_length(&dma->ctx->apu.dmc, val);
		break;
	case 0x4014: /* OAM_DMA */
		oam_write(dma, val);
		break;
	default:
		break;
	}
}
