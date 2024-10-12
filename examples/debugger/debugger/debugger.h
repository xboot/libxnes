#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnes.h>

void debugger_init(struct xnes_ctx_t * ctx);
void debugger_exit(struct xnes_ctx_t * ctx);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUGGER_H__ */
