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

#include <slist.h>
#include <context.h>
#include <command.h>

static void usage(void)
{
	shell_printf("usage:\r\n");
	shell_printf("    help [command ...]\r\n");
}

static int do_help(struct xnes_ctx_t * ctx, int argc, char ** argv)
{
	struct command_t * pos, * n;
	struct command_t * c;
	struct slist_t * sl, * e;
	int i, j, k = 0;

	if(argc == 1)
	{
		sl = slist_alloc();
		list_for_each_entry_safe(pos, n, &__command_list, list)
		{
			j = strlen(pos->name);
			if(j > k)
				k = j;
			slist_add(sl, pos, "%s", pos->name);
		}
		slist_sort(sl);
		slist_for_each_entry(e, sl)
		{
			pos = (struct command_t *)e->priv;
			shell_printf(" %s%*s - %s\r\n", pos->name, k - strlen(pos->name), "", pos->desc);
		}
		slist_free(sl);
	}
	else
	{
		for(i = 1; i < argc; i++)
		{
			c = search_command(argv[i]);
			if(c)
			{
				shell_printf("%s - %s\r\n", c->name, c->desc);
				if(c->usage)
					c->usage();
			}
			else
			{
				shell_printf("unknown command '%s' - try 'help' for list all of commands\r\n", argv[i]);
			}
		}
	}
	return 0;
}

struct command_t cmd_help = {
	.name	= "help",
	.desc	= "show online help about command",
	.usage	= usage,
	.exec	= do_help,
};
