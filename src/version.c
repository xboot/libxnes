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

#include <version.h>

#define XNES_VERSION_MAJOY		1
#define XNES_VERSION_MINIOR		0
#define XNES_VERSION_PATCH		0

#define VERSION_TO_STRING(major, minor, patch) \
	#major"."#minor"."#patch

#define XNES_VERSION_STRING(major, minor, patch) \
	VERSION_TO_STRING(major, minor, patch)

int xnes_version(void)
{
	return (XNES_VERSION_MAJOY * 10000) + (XNES_VERSION_MINIOR * 100) + (XNES_VERSION_PATCH * 1);
}

const char * xnes_banner(void)
{
	return ("V"XNES_VERSION_STRING(XNES_VERSION_MAJOY, XNES_VERSION_MINIOR, XNES_VERSION_PATCH)" ("__DATE__" - "__TIME__")");
}
