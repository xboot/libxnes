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

struct xnes_ctx_t * xnes_ctx_alloc(const void * buf, size_t len)
{
	struct xnes_ctx_t * ctx;

	ctx = xnes_malloc(sizeof(struct xnes_ctx_t));
	if(!ctx)
		return NULL;

	xnes_memset(ctx, 0, sizeof(struct xnes_ctx_t));
	ctx->cartridge = xnes_cartridge_alloc(buf, len, ctx);
	if(!ctx->cartridge)
	{
		xnes_free(ctx);
		return NULL;
	}

	xnes_cpu_init(&ctx->cpu, ctx);
	xnes_dma_init(&ctx->dma, ctx);
	xnes_ppu_init(&ctx->ppu, ctx);
	xnes_apu_init(&ctx->apu, ctx);
	xnes_controller_init(&ctx->ctl, ctx);

	return ctx;
}

void xnes_ctx_free(struct xnes_ctx_t * ctx)
{
	if(ctx)
	{
		if(ctx->cartridge)
			xnes_cartridge_free(ctx->cartridge);
		xnes_free(ctx);
	}
}

void xnes_reset(struct xnes_ctx_t * ctx)
{
	if(ctx)
	{
		xnes_cpu_reset(&ctx->cpu);
		xnes_dma_reset(&ctx->dma);
		xnes_ppu_reset(&ctx->ppu);
		xnes_apu_reset(&ctx->apu);
		xnes_controller_reset(&ctx->ctl);
	}
}

void xnes_set_debugger(struct xnes_ctx_t * ctx, int (*debugger)(struct xnes_ctx_t *))
{
	if(ctx)
		ctx->cpu.debugger = debugger;
}

void xnes_set_audio(struct xnes_ctx_t * ctx, void * data, void (*cb)(void *, float), int rate)
{
	if(ctx)
		xnes_apu_set_audio_callback(&ctx->apu, data, cb, rate);
}

void xnes_set_speed(struct xnes_ctx_t * ctx, float speed)
{
	if(ctx && ctx->cartridge)
	{
		ctx->cartridge->cpu_rate_adjusted = ctx->cartridge->cpu_rate * speed;
		ctx->cartridge->cpu_period_adjusted = 1000000000 / ctx->cartridge->cpu_rate_adjusted;
	}
}

static inline int xnes_step(struct xnes_ctx_t * ctx)
{
	int cycles = xnes_cpu_step(&ctx->cpu);

	for(int i = 0; i < cycles; i++)
	{
		xnes_ppu_step(&ctx->ppu);
		xnes_ppu_step(&ctx->ppu);
		xnes_ppu_step(&ctx->ppu);
		xnes_apu_step(&ctx->apu);
	}
	return cycles;
}

uint64_t xnes_step_frame(struct xnes_ctx_t * ctx)
{
	int cycles = 0;
	int frame = ctx->ppu.frame;

	while(frame == ctx->ppu.frame)
	{
		cycles += xnes_step(ctx);
	}
	return (uint64_t)ctx->cartridge->cpu_period_adjusted * cycles;
}

int xnes_save(struct xnes_ctx_t * ctx, void * buf)
{
	return 0;
}

void xnes_restore(struct xnes_ctx_t * ctx, void * buf, int len)
{
}
