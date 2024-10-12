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

#include <debugger.h>
#include <context.h>
#include <command.h>

static void usage(void)
{
	shell_printf("usage:\r\n");
	shell_printf("    echo [option] [string]...\r\n");
	shell_printf("    -n    do not output the trailing newline\r\n");
}

static int do_echo(struct xnes_ctx_t * ctx, int argc, char ** argv)
{
	int nflag = 0;

	if(*++argv && !strcmp(*argv, "-n"))
	{
		++argv;
		nflag = 1;
	}

	while(*argv)
	{
		shell_printf("%s", *argv);
		if(*++argv)
			shell_putchar(' ');
	}

	if(nflag == 0)
		shell_printf("\r\n");

	return 0;
}

struct command_t cmd_echo = {
	.name	= "echo",
	.desc	= "echo the string to standard output",
	.usage	= usage,
	.exec	= do_echo,
};
