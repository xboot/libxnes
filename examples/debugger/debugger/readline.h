#ifndef __READLINE_H__
#define __READLINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xdef.h>

enum esc_state_t {
	ESC_STATE_NORMAL,
	ESC_STATE_ESC,
	ESC_STATE_CSI,
};

struct rl_buf_t {
	char * prompt;
	uint32_t * buf;
	uint32_t * cut;
	int bsize;
	int pos;
	int len;

	enum esc_state_t state;
	int params[8];
	int num_params;
	char utf8[32];
	int usize;
};

struct rl_buf_t * rl_alloc(const char * prompt);
void rl_free(struct rl_buf_t * rl);
char * shell_readline(struct rl_buf_t * rl);

#ifdef __cplusplus
}
#endif

#endif /* __READLINE_H__ */
