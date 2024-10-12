#ifndef __SHELL_H__
#define __SHELL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnes.h>

int shell_system(struct xnes_ctx_t * ctx, const char * cmdline);

#ifdef __cplusplus
}
#endif

#endif /* __SHELL_H__ */
