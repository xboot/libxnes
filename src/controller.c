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

#include <xnes.h>

void xnes_controller_init(struct xnes_controller_t * ctl, struct xnes_ctx_t * ctx)
{
	ctl->ctx = ctx;
	xnes_controller_reset(ctl);
}

void xnes_controller_reset(struct xnes_controller_t * ctl)
{
	ctl->turbo_count = 0x0;
	ctl->turbo_speed = XNES_CONTROLLER_TURBO_SPEED_NORMAL;
	ctl->strobe = 0x0;
	ctl->p1 = 0x0;
	ctl->p1_turbo = 0x0;
	ctl->p1_index = 0x0;
	ctl->p2 = 0x0;
	ctl->p2_turbo = 0x0;
	ctl->p2_index = 0x0;
	ctl->x = 0;
	ctl->y = 0;
	ctl->trigger = 0;
}

void xnes_controller_set_turbo_speed(struct xnes_controller_t * ctl, uint8_t speed)
{
	ctl->turbo_speed = speed;
}

uint8_t xnes_controller_read_register(struct xnes_controller_t * ctl, uint16_t addr)
{
	uint8_t val = 0;

	switch(addr)
	{
	case 0x4016:
		if(ctl->strobe & (0x1 << 0))
			val = (ctl->p1 & 0x80) ? 1 : 0;
		else
		{
			if(ctl->p1_index < 8)
			{
				if(ctl->p1_turbo & (0x80 >> ctl->p1_index))
					val = (ctl->turbo_count & ctl->turbo_speed) ? 1 : 0;
				else
					val = (ctl->p1 & (0x80 >> ctl->p1_index)) ? 1 : 0;
				ctl->p1_index++;
			}
			else
				val = 1;
		}
		break;
	case 0x4017:
		if(ctl->strobe & (0x1 << 0))
			val = (ctl->p2 & 0x80) ? 1 : 0;
		else
		{
			if(ctl->p2_index < 8)
			{
				if(ctl->p2_turbo & (0x80 >> ctl->p2_index))
					val = (ctl->turbo_count & ctl->turbo_speed) ? 1 : 0;
				else
					val = (ctl->p2 & (0x80 >> ctl->p2_index)) ? 1 : 0;
				ctl->p2_index++;
			}
			else
				val = 1;
			if((ctl->x != 0) && (ctl->y != 0))
			{
				if(xnes_ppu_is_white_pixel(&ctl->ctx->ppu, ctl->x, ctl->y))
					val |= (0 << 3);
				else
					val |= (1 << 3);
				if(ctl->trigger)
					val |= (1 << 4);
			}
		}
		break;
	default:
		break;
	}
	return val;
}

void xnes_controller_write_register(struct xnes_controller_t * ctl, uint16_t addr, uint8_t val)
{
	switch(addr)
	{
	case 0x4016:
		ctl->turbo_count++;
		ctl->strobe = val & 0x7;
		if(ctl->strobe & (0x1 << 0))
		{
			ctl->p1_index = 0;
			ctl->p2_index = 0;
		}
		break;
	default:
		break;
	}
}

void xnes_controller_joystick_p1(struct xnes_controller_t * ctl, uint8_t down, uint8_t up)
{
	ctl->p1 |= down;
	ctl->p1 &= ~up;
}

void xnes_controller_joystick_p2(struct xnes_controller_t * ctl, uint8_t down, uint8_t up)
{
	ctl->p2 |= down;
	ctl->p2 &= ~up;
}

void xnes_controller_joystick_p1_turbo(struct xnes_controller_t * ctl, uint8_t down, uint8_t up)
{
	ctl->p1_turbo |= down;
	ctl->p1_turbo &= ~up;
}

void xnes_controller_joystick_p2_turbo(struct xnes_controller_t * ctl, uint8_t down, uint8_t up)
{
	ctl->p2_turbo |= down;
	ctl->p2_turbo &= ~up;
}

void xnes_controller_zapper(struct xnes_controller_t * ctl, uint8_t x, uint8_t y, uint8_t trigger)
{
	ctl->x = x;
	ctl->y = y;
	ctl->trigger = trigger ? 1 : 0;
}
