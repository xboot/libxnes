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

#include <command.h>
#include <context.h>
#include <readline.h>
#include <shell.h>

static char ** shell_splitstring(char * cmdline, char * delim)
{
	int bufsz = 16;
	int pos = 0;
	char ** tokens;
	char * tok;
	char * p;

	tokens = malloc(bufsz * sizeof(char *));
	if(tokens)
	{
		tok = strtok_r(cmdline, delim, &p);
		while(tok != NULL)
		{
			tokens[pos] = tok;
			pos++;
			if(pos >= bufsz)
			{
				bufsz <<= 1;
				tokens = realloc(tokens, bufsz * sizeof(char *));
			}
			tok = strtok_r(NULL, delim, &p);
		}
		tokens[pos] = NULL;
	}
	return tokens;
}

int shell_system(const char * cmdline)
{
	int ret = 0;

	char * str = strdup(cmdline);
	if(str)
	{
		char ** cmds = shell_splitstring(str, ";");
		if(cmds)
		{
			for(int i = 0; cmds[i] != NULL; i++)
			{
				char ** argv = shell_splitstring(cmds[i], " \t\r\n\a");
				if(argv)
				{
					int argc = 0;
					while(argv[argc] != NULL)
						argc++;
					if(argc > 0)
					{
						struct command_t * c = search_command(argv[0]);
						if(c)
						{
							ret = c->exec(argc, argv);
						}
						else
						{
							ret = -1;
							shell_printf(" could not found '%s' command\r\n", argv[0]);
						}
					}
					free(argv);
				}
				if(ret < 0)
					break;
			}
			free(cmds);
		}
		free(str);
	}
	return ret;
}
