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

#include <termios.h>
#include <context.h>
#include <command.h>
#include <ctrlc.h>
#include <hexdump.h>
#include <lsort.h>
#include <charset.h>
#include <readline.h>
#include <shell.h>
#include <debugger.h>

static struct termios tconfig;
static struct rl_buf_t * rl;

static int debugger_callback(struct xnes_ctx_t * ctx)
{
	char * p = shell_readline(rl);
	if(p)
	{
		shell_system(p);
		free(p);
		shell_printf("%s", rl->prompt);
	}
	return 0;
}

void debugger_init(struct xnes_ctx_t * ctx)
{
	struct termios ta;

	tcgetattr(0, &tconfig);
	ta = tconfig;
	ta.c_lflag &= (~ICANON & ~ECHO & ~ISIG);
	ta.c_iflag &= (~IXON & ~ICRNL);
	ta.c_oflag |= (ONLCR);
	ta.c_cc[VMIN] = 1;
	ta.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &ta);

	rl = rl_alloc("==> ");
	xnes_set_debugger(ctx, debugger_callback);
}

void debugger_exit(struct xnes_ctx_t * ctx)
{
	fflush(stdout);
	tcsetattr(0, TCSANOW, &tconfig);

	rl_free(rl);
}
