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
#include <lsort.h>
#include <charset.h>
#include <readline.h>
#include <shell.h>
#include <debugger.h>

static struct termios tconfig;
static struct rl_buf_t * rl;
static int debugger_step_flag	= 0;
static int debugger_step_count	= 0;
static uint16_t break_point		= 0x0000;

static int debugger_callback(struct xnes_ctx_t * ctx)
{
	char * p = shell_readline(rl);
	if(p)
	{
		shell_printf("\r\n");
		if(p[0] != '\0')
			shell_system(ctx, p);
		free(p);
		shell_printf("%s", rl->prompt);
	}
	if(debugger_step_flag)
	{
		if(debugger_step_count > 0)
		{
			--debugger_step_count;
			return 0;
		}
		else
			return 1;
	}
	else
	{
		if(ctx->cpu.pc == break_point)
			return 1;
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
	rl = rl_alloc("--> ");
	xnes_set_debugger(ctx, debugger_callback);

	register_command(&cmd_breakpoint);
	register_command(&cmd_dasm);
	register_command(&cmd_dump);
	register_command(&cmd_exit);
	register_command(&cmd_flag);
	register_command(&cmd_help);
	register_command(&cmd_pause);
	register_command(&cmd_reset);
	register_command(&cmd_run);
	register_command(&cmd_step);
}

void debugger_exit(struct xnes_ctx_t * ctx)
{
	rl_free(rl);
	fflush(stdout);
	tcsetattr(0, TCSANOW, &tconfig);

	unregister_command(&cmd_breakpoint);
	unregister_command(&cmd_dasm);
	unregister_command(&cmd_dump);
	unregister_command(&cmd_exit);
	unregister_command(&cmd_flag);
	unregister_command(&cmd_help);
	unregister_command(&cmd_pause);
	unregister_command(&cmd_reset);
	unregister_command(&cmd_run);
	unregister_command(&cmd_step);
}

void debugger_run(struct xnes_ctx_t * ctx)
{
	debugger_step_flag = 0;
}

void debugger_step(struct xnes_ctx_t * ctx, int step)
{
	debugger_step_flag = 1;
	debugger_step_count = step > 0 ? step : 0;
}

void debugger_set_breakpoint(struct xnes_ctx_t * ctx, uint16_t bp)
{
	break_point = bp;
}

uint16_t debugger_get_breakpoint(struct xnes_ctx_t * ctx)
{
	return break_point;
}
