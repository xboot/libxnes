#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnes.h>

void debugger_init(struct xnes_ctx_t * ctx);
void debugger_exit(struct xnes_ctx_t * ctx);

void debugger_run(struct xnes_ctx_t * ctx);
void debugger_step(struct xnes_ctx_t * ctx, int step);
void debugger_breakpoint(struct xnes_ctx_t * ctx, uint16_t bp);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUGGER_H__ */
