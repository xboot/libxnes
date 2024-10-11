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

#include <hexdump.h>

static inline unsigned char tohex(unsigned char v)
{
	return ((v < 10) ? (v + '0') : (v - 10 + 'a'));
}

void hexdump(void (*output)(char), unsigned long base, void * buf, int len)
{
	if(output)
	{
		unsigned long o = (unsigned long)base;
		unsigned char * p = (unsigned char *)buf;
		for(int n = 0; n < len; n += 16, o += 16)
		{
			output(tohex((o >> 28) & 0xf));
			output(tohex((o >> 24) & 0xf));
			output(tohex((o >> 20) & 0xf));
			output(tohex((o >> 16) & 0xf));
			output(tohex((o >> 12) & 0xf));
			output(tohex((o >>  8) & 0xf));
			output(tohex((o >>  4) & 0xf));
			output(tohex((o >>  0) & 0xf));
			output(':');
			output(' ');
			for(int i = 0; i < 16; i++)
			{
				if(n + i < len)
				{
					unsigned char c = p[n + i];
					output(tohex((c >> 4) & 0xf));
					output(tohex((c >> 0) & 0xf));
					output(' ');
				}
				else
				{
					output(' ');
					output(' ');
					output(' ');
				}
			}
			output('|');
			for(int i = 0; i < 16; i++)
			{
				if(n + i >= len)
					output(' ');
				else
				{
					unsigned char c = p[n + i];
					if(((unsigned int)c - 0x20) < 0x5f)
						output(c);
					else
						output('.');
				}
			}
			output('\r');
			output('\n');
		}
	}
}
