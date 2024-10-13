#ifndef __XNES_H__
#define __XNES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cpu.h>
#include <dma.h>
#include <ppu.h>
#include <apu.h>
#include <controller.h>
#include <cartridge.h>

struct xnes_ctx_t * xnes_ctx_alloc(const void * buf, size_t len);
void xnes_ctx_free(struct xnes_ctx_t * ctx);
void xnes_reset(struct xnes_ctx_t * ctx);
void xnes_set_debugger(struct xnes_ctx_t * ctx, int (*debugger)(struct xnes_ctx_t *));
void xnes_set_audio(struct xnes_ctx_t * ctx, void * data, void (*cb)(void *, float), int rate);
void xnes_set_speed(struct xnes_ctx_t * ctx, float speed);
uint64_t xnes_step_frame(struct xnes_ctx_t * ctx);

struct xnes_state_t {
	struct xnes_ctx_t * ctx;
	unsigned char * buffer;
	uint32_t length;
	uint32_t count;
	uint32_t in;
	uint32_t out;
};
struct xnes_state_t * xnes_state_alloc(struct xnes_ctx_t * ctx, int count);
void xnes_state_free(struct xnes_state_t * state);
void xnes_state_push(struct xnes_state_t * state);
void xnes_state_pop(struct xnes_state_t * state);

#ifdef __cplusplus
}
#endif

#endif /* __XNES_H__ */
