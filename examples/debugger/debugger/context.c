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

struct shell_history_t {
	struct list_head head;
	uint32_t * ucs4;
};

struct shell_context_t {
	struct list_head history_list;
	struct list_head * history_current;
	ssize_t (*read)(void * buf, size_t count);
	ssize_t (*write)(void * buf, size_t count);
};

static ssize_t shell_stdio_read(void * buf, size_t count)
{
	return 0;
}

static ssize_t shell_stdio_write(void * buf, size_t count)
{
	return 0;
}

static struct shell_context_t * shell_context_get(void)
{
	static struct shell_context_t * ctx = NULL;

	if(!ctx)
	{
		ctx = malloc(sizeof(struct shell_context_t));
		if(ctx)
		{
			init_list_head(&ctx->history_list);
			ctx->history_current = &ctx->history_list;
			ctx->read = shell_stdio_read;
			ctx->write = shell_stdio_write;
		}
	}
	return ctx;
}

uint32_t * shell_history_prev(void)
{
	struct shell_context_t * ctx = shell_context_get();

	if(list_empty_careful(&ctx->history_list))
	{
		ctx->history_current = &ctx->history_list;
		return NULL;
	}

	struct list_head * list = ctx->history_current->prev;
	if(list != &ctx->history_list)
	{
		ctx->history_current = list;
		return ((struct shell_history_t *)list_entry(list, struct shell_history_t, head))->ucs4;
	}
	return NULL;
}

uint32_t * shell_history_next(void)
{
	struct shell_context_t * ctx = shell_context_get();

	if(list_empty_careful(&ctx->history_list))
	{
		ctx->history_current = &ctx->history_list;
		return NULL;
	}
	if(ctx->history_current == &ctx->history_list)
		return NULL;

	struct list_head * list = ctx->history_current->next;
	ctx->history_current = list;
	if(list != &ctx->history_list)
		return ((struct shell_history_t *)list_entry(list, struct shell_history_t, head))->ucs4;
	return NULL;
}

void shell_history_add(uint32_t * ucs4, int len)
{
	struct shell_context_t * ctx = shell_context_get();

	ctx->history_current = &ctx->history_list;
	if(!ucs4 || (len <= 0))
		return;

	if(!list_empty_careful(&ctx->history_list))
	{
		struct shell_history_t * entry = list_last_entry(&ctx->history_list, struct shell_history_t, head);
		uint32_t * sc = entry->ucs4;
		for(sc = entry->ucs4; *sc != '\0'; ++sc);
		if(sc - entry->ucs4 == len)
		{
			if(memcmp(entry->ucs4, ucs4, len * sizeof(uint32_t)) == 0)
				return;
		}
	}

	if(!list_empty_careful(&ctx->history_list))
	{
		struct shell_history_t * pos;
		int count = 0;
		list_for_each_entry(pos, &ctx->history_list, head)
		{
			count++;
		}
		if(count >= 32)
		{
			struct shell_history_t * entry = (struct shell_history_t *)list_first_entry_or_null(&ctx->history_list, struct shell_history_t, head);
			if(entry)
			{
				list_del(&entry->head);
				free(entry->ucs4);
				free(entry);
			}
			ctx->history_current = &ctx->history_list;
		}
	}

	struct shell_history_t * entry = malloc(sizeof(struct shell_history_t));
	if(!entry)
		return;

	entry->ucs4 = malloc((len + 1) * sizeof(uint32_t));
	if(!entry->ucs4)
	{
		free(entry);
		return;
	}
	for(int i = 0; i < len; i++)
		entry->ucs4[i] = ucs4[i];
	entry->ucs4[len] = '\0';
	list_add_tail(&entry->head, &ctx->history_list);
}

int shell_getchar(void)
{
	struct shell_context_t * ctx = shell_context_get();
	unsigned char ch;

	if(ctx->read && (ctx->read(&ch, 1) == 1))
		return (int)ch;
	return -1;
}

int shell_putchar(int c)
{
	struct shell_context_t * ctx = shell_context_get();
	unsigned char ch = c & 0xff;

	if(ctx->write && (ctx->write(&ch, 1) == 1))
		return ch;
	return -1;
}

int shell_vprintf(const char * fmt, va_list ap)
{
	struct shell_context_t * ctx = shell_context_get();
	char * p = NULL;
	int len;

	len = vasprintf(&p, fmt, ap);
	if(p && (len > 0))
	{
		if(ctx->write)
			ctx->write(p, len);
		free(p);
	}
	return len;
}

int shell_printf(const char * fmt, ...)
{
	va_list ap;
	int len;

	va_start(ap, fmt);
	len = shell_vprintf(fmt, ap);
	va_end(ap);

	return len;
}
