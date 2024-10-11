#ifndef __XNES_H__
#define __XNES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cpu.h>
#include <ppu.h>
#include <apu.h>
#include <controller.h>
#include <cartridge.h>
#include <version.h>

struct xnes_ctx_t * xnes_ctx_alloc(const void * buf, size_t len);
void xnes_ctx_free(struct xnes_ctx_t * ctx);
void xnes_reset(struct xnes_ctx_t * ctx);
void xnes_set_debugger(struct xnes_ctx_t * ctx, int (*debugger)(struct xnes_ctx_t *));
void xnes_set_audio(struct xnes_ctx_t * ctx, void * data, void (*cb)(void *, float), int rate);
void xnes_speed(struct xnes_ctx_t * ctx, float speed);
uint64_t xnes_step_frame(struct xnes_ctx_t * ctx);
int xnes_save(struct xnes_ctx_t * ctx, void * buf);
void xnes_restore(struct xnes_ctx_t * ctx, void * buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* __XNES_H__ */
