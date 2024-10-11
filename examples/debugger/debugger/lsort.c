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

#include <lsort.h>

static struct list_head * merge(void * priv, int (*cmp)(void * priv, struct list_head * a, struct list_head * b), struct list_head * a, struct list_head * b)
{
	struct list_head head, * tail = &head;

	while(a && b)
	{
		if((*cmp)(priv, a, b) <= 0)
		{
			tail->next = a;
			a = a->next;
		}
		else
		{
			tail->next = b;
			b = b->next;
		}
		tail = tail->next;
	}
	tail->next = a ? a : b;
	return head.next;
}

static void merge_and_restore_back_links(void * priv, int (*cmp)(void * priv, struct list_head * a, struct list_head * b), struct list_head * head, struct list_head * a, struct list_head * b)
{
	struct list_head * tail = head;
	unsigned char count = 0;

	while(a && b)
	{
		if((*cmp)(priv, a, b) <= 0)
		{
			tail->next = a;
			a->prev = tail;
			a = a->next;
		}
		else
		{
			tail->next = b;
			b->prev = tail;
			b = b->next;
		}
		tail = tail->next;
	}
	tail->next = a ? a : b;

	do {
		if(unlikely(!(++count)))
			(*cmp)(priv, tail->next, tail->next);
		tail->next->prev = tail;
		tail = tail->next;
	} while(tail->next);

	tail->next = head;
	head->prev = tail;
}

void lsort(void * priv, struct list_head * head, int (*cmp)(void * priv, struct list_head * a, struct list_head * b))
{
	struct list_head * part[20 + 1];
	struct list_head * list;
	int maxlev = 0;
	int lev;

	if(list_empty(head))
		return;

	memset(part, 0, sizeof(part));
	head->prev->next = NULL;
	list = head->next;

	while(list)
	{
		struct list_head * cur = list;
		list = list->next;
		cur->next = NULL;

		for(lev = 0; part[lev]; lev++)
		{
			cur = merge(priv, cmp, part[lev], cur);
			part[lev] = NULL;
		}
		if(lev > maxlev)
		{
			if(unlikely(lev >= ARRAY_SIZE(part) - 1))
				lev--;
			maxlev = lev;
		}
		part[lev] = cur;
	}
	for(lev = 0; lev < maxlev; lev++)
	{
		if(part[lev])
			list = merge(priv, cmp, part[lev], list);
	}
	merge_and_restore_back_links(priv, cmp, head, part[maxlev], list);
}
