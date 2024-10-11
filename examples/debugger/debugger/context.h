#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xdef.h>

uint32_t * shell_history_prev(void);
uint32_t * shell_history_next(void);
void shell_history_add(uint32_t * ucs4, int len);

int shell_getchar(void);
int shell_putchar(int c);
int shell_vprintf(const char * fmt, va_list ap);
int shell_printf(const char * fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __CONTEXT_H__ */
