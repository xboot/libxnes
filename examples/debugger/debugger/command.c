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

struct list_head __command_list = {
	.next = &__command_list,
	.prev = &__command_list,
};

struct command_t * search_command(const char * name)
{
	struct command_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &__command_list, list)
	{
		if(strcmp(pos->name, name) == 0)
			return pos;
	}
	return NULL;
}

int register_command(struct command_t * cmd)
{
	if(!cmd || !cmd->name || !cmd->exec)
		return FALSE;

	if(search_command(cmd->name))
		return FALSE;

	list_add_tail(&cmd->list, &__command_list);
	return TRUE;
}

int unregister_command(struct command_t * cmd)
{
	if(!cmd || !cmd->name)
		return FALSE;

	list_del(&cmd->list);
	return TRUE;
}
