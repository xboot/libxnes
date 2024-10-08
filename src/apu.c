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

#include <cpu.h>
#include <apu.h>

static const uint8_t length_table[] = {
	10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
	12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30,
};

static const uint8_t duty_table[4][8] = {
	{ 0, 1, 0, 0, 0, 0, 0, 0 },
	{ 0, 1, 1, 0, 0, 0, 0, 0 },
	{ 0, 1, 1, 1, 1, 0, 0, 0 },
	{ 1, 0, 0, 1, 1, 1, 1, 1 },
};

static const uint8_t triangle_table[] = {
	15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
};

static const uint16_t noise_table[] = {
	4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068,
};

static const uint8_t dmc_table[] = {
	214, 190, 170, 160, 143, 127, 113, 107, 95, 80, 71, 64, 53, 42, 36, 27,
};

static const float pulse_table[31] = {
	0.000000, 0.011609, 0.022939, 0.034001, 0.044803, 0.055355, 0.065665, 0.075741,
	0.085591, 0.095224, 0.104645, 0.113862, 0.122882, 0.131710, 0.140353, 0.148816,
	0.157105, 0.165226, 0.173183, 0.180981, 0.188626, 0.196120, 0.203470, 0.210679,
	0.217751, 0.224689, 0.231499, 0.238182, 0.244744, 0.251186, 0.257513,
};

static const float tnd_table[203] = {
	0.000000, 0.006700, 0.013345, 0.019936, 0.026474, 0.032959, 0.039393, 0.045775,
	0.052106, 0.058386, 0.064618, 0.070800, 0.076934, 0.083020, 0.089058, 0.095050,
	0.100996, 0.106896, 0.112751, 0.118561, 0.124327, 0.130049, 0.135728, 0.141365,
	0.146959, 0.152512, 0.158024, 0.163494, 0.168925, 0.174315, 0.179666, 0.184978,
	0.190252, 0.195487, 0.200684, 0.205845, 0.210968, 0.216054, 0.221105, 0.226120,
	0.231099, 0.236043, 0.240953, 0.245828, 0.250669, 0.255477, 0.260252, 0.264993,
	0.269702, 0.274379, 0.279024, 0.283638, 0.288220, 0.292771, 0.297292, 0.301782,
	0.306242, 0.310673, 0.315074, 0.319446, 0.323789, 0.328104, 0.332390, 0.336649,
	0.340879, 0.345083, 0.349259, 0.353408, 0.357530, 0.361626, 0.365696, 0.369740,
	0.373759, 0.377752, 0.381720, 0.385662, 0.389581, 0.393474, 0.397344, 0.401189,
	0.405011, 0.408809, 0.412584, 0.416335, 0.420064, 0.423770, 0.427454, 0.431115,
	0.434754, 0.438371, 0.441966, 0.445540, 0.449093, 0.452625, 0.456135, 0.459625,
	0.463094, 0.466543, 0.469972, 0.473380, 0.476769, 0.480138, 0.483488, 0.486818,
	0.490129, 0.493421, 0.496694, 0.499948, 0.503184, 0.506402, 0.509601, 0.512782,
	0.515946, 0.519091, 0.522219, 0.525330, 0.528423, 0.531499, 0.534558, 0.537601,
	0.540626, 0.543635, 0.546627, 0.549603, 0.552563, 0.555507, 0.558434, 0.561346,
	0.564242, 0.567123, 0.569988, 0.572838, 0.575673, 0.578493, 0.581298, 0.584088,
	0.586863, 0.589623, 0.592370, 0.595101, 0.597819, 0.600522, 0.603212, 0.605887,
	0.608549, 0.611197, 0.613831, 0.616452, 0.619059, 0.621653, 0.624234, 0.626802,
	0.629357, 0.631899, 0.634428, 0.636944, 0.639448, 0.641939, 0.644418, 0.646885,
	0.649339, 0.651781, 0.654212, 0.656630, 0.659036, 0.661431, 0.663813, 0.666185,
	0.668544, 0.670893, 0.673229, 0.675555, 0.677869, 0.680173, 0.682465, 0.684746,
	0.687017, 0.689276, 0.691525, 0.693763, 0.695991, 0.698208, 0.700415, 0.702611,
	0.704797, 0.706973, 0.709139, 0.711294, 0.713440, 0.715576, 0.717702, 0.719818,
	0.721924, 0.724021, 0.726108, 0.728186, 0.730254, 0.732313, 0.734362, 0.736402,
	0.738433, 0.740455, 0.742468,
};

static inline void pulse_write_control(struct xnes_apu_pulse_t * p, uint8_t value)
{
	p->duty_mode = (value >> 6) & 0x3;
	p->length_enabled = (value & 0x20) ? 0 : 1;
	p->envelope_loop = (value & 0x20) ? 1 : 0;
	p->envelope_enabled = (value & 0x10) ? 0 : 1;
	p->envelope_period = value & 0xf;
	p->constant_volume = value & 0xf;
	p->envelope_start = 1;
}

static inline void pulse_write_sweep(struct xnes_apu_pulse_t * p, uint8_t value)
{
	p->sweep_enabled = (value & 0x80) ? 1 : 0;
	p->sweep_period = ((value >> 4) & 0x7) + 1;
	p->sweep_negate = (value & 0x08) ? 1 : 0;
	p->sweep_shift = value & 0x7;
	p->sweep_reload = 1;
}

static inline void pulse_write_timer_low(struct xnes_apu_pulse_t * p, uint8_t value)
{
	p->timer_period = (p->timer_period & 0xff00) | ((uint16_t)value);
}

static inline void pulse_write_timer_high(struct xnes_apu_pulse_t * p, uint8_t value)
{
	p->length_value = length_table[((value >> 3) & 0x1f)];
	p->timer_period = (p->timer_period & 0x00ff) | (((uint16_t)(value & 0x7)) << 8);
	p->envelope_start = 1;
	p->duty_value = 0;
}

static inline void pulse_step_timer(struct xnes_apu_pulse_t * p)
{
	if(p->timer_value == 0)
	{
		p->timer_value = p->timer_period;
		p->duty_value = (p->duty_value + 1) & 0x7;
	}
	else
		p->timer_value--;
}

static inline void pulse_step_envelope(struct xnes_apu_pulse_t * p)
{
	if(p->envelope_start)
	{
		p->envelope_volume = 15;
		p->envelope_value = p->envelope_period;
		p->envelope_start = 0;
	}
	else if(p->envelope_value > 0)
	{
		p->envelope_value--;
	}
	else
	{
		if(p->envelope_volume > 0)
			p->envelope_volume--;
		else if(p->envelope_loop)
			p->envelope_volume = 15;
		p->envelope_value = p->envelope_period;
	}
}

static inline void pulse_sweep(struct xnes_apu_pulse_t * p)
{
	uint16_t delta = p->timer_period >> p->sweep_shift;
	if(p->sweep_negate)
	{
		p->timer_period -= delta;
		if(p->channel == 1)
			p->timer_period--;
	}
	else
		p->timer_period += delta;
}

static inline void pulse_step_sweep(struct xnes_apu_pulse_t * p)
{
	if(p->sweep_reload)
	{
		if(p->sweep_enabled && p->sweep_value == 0)
			pulse_sweep(p);
		p->sweep_value = p->sweep_period;
		p->sweep_reload = 0;
	}
	else if(p->sweep_value > 0)
	{
		p->sweep_value--;
	}
	else
	{
		if(p->sweep_enabled)
			pulse_sweep(p);
		p->sweep_value = p->sweep_period;
	}
}

static inline void pulse_step_length(struct xnes_apu_pulse_t * p)
{
	if(p->length_enabled && (p->length_value > 0))
		p->length_value--;
}

static inline uint8_t pulse_output(struct xnes_apu_pulse_t * p)
{
	if(!p->enabled)
		return 0;
	if(p->length_value == 0)
		return 0;
	if(duty_table[p->duty_mode][p->duty_value] == 0)
		return 0;
	if((p->timer_period < 8) || (p->timer_period > 0x7ff))
		return 0;
	if(p->envelope_enabled)
		return p->envelope_volume;
	else
		return p->constant_volume;
}

static inline void triangle_write_control(struct xnes_apu_triangle_t * t, uint8_t value)
{
	t->length_enabled = (value & 0x80) ? 0 : 1;
	t->counter_period = value & 0x7f;
}

static inline void triangle_write_timer_low(struct xnes_apu_triangle_t * t, uint8_t value)
{
	t->timer_period = (t->timer_period & 0xff00) | ((uint16_t)value);
}

static inline void triangle_write_timer_high(struct xnes_apu_triangle_t * t, uint8_t value)
{
	t->length_value = length_table[(value >> 3) & 0x1f];
	t->timer_period = (t->timer_period & 0x00ff) | (((uint16_t)(value & 0x7)) << 8);
	t->timer_value = t->timer_period;
	t->counter_reload = 1;
}

static inline void triangle_step_timer(struct xnes_apu_triangle_t * t)
{
	if(t->timer_value == 0)
	{
		t->timer_value = t->timer_period;
		if((t->length_value > 0) && (t->counter_value > 0))
			t->duty_value = (t->duty_value + 1) % 32;
	}
	else
		t->timer_value--;
}

static inline void triangle_step_length(struct xnes_apu_triangle_t * t)
{
	if(t->length_enabled && (t->length_value > 0))
		t->length_value--;
}

static inline void triangle_step_counter(struct xnes_apu_triangle_t * t)
{
	if(t->counter_reload)
		t->counter_value = t->counter_period;
	else if(t->counter_value > 0)
		t->counter_value--;
	if(t->length_enabled)
		t->counter_reload = 0;
}

static inline uint8_t triangle_output(struct xnes_apu_triangle_t * t)
{
	if(!t->enabled)
		return 0;
	if(t->timer_period < 3)
		return 0;
	if(t->length_value == 0)
		return 0;
	if(t->counter_value == 0)
		return 0;
	return triangle_table[t->duty_value];
}

static inline void noise_write_control(struct xnes_apu_noise_t * n, uint8_t value)
{
	n->length_enabled = (value & 0x20) ? 0 : 1;
	n->envelope_loop = (value & 0x20) ? 1 : 0;
	n->envelope_enabled = (value & 0x10) ? 0 : 1;
	n->envelope_period = value & 0xf;
	n->constant_volume = value & 0xf;
	n->envelope_start = 1;
}

static inline void noise_write_period(struct xnes_apu_noise_t * n, uint8_t value)
{
	n->mode = (value & 0x80) ? 1 : 0;
	n->timer_period = noise_table[value & 0xf];
}

static inline void noise_write_length(struct xnes_apu_noise_t * n, uint8_t value)
{
	n->length_value = length_table[(value >> 3) & 0x1f];
	n->envelope_start = 1;
}

static inline void noise_step_timer(struct xnes_apu_noise_t * n)
{
	if(n->timer_value == 0)
	{
		n->timer_value = n->timer_period;
		uint8_t shift = 0;
		if(n->mode)
			shift = 6;
		else
			shift = 1;
		uint16_t b1 = n->shift_register & 0x1;
		uint16_t b2 = (n->shift_register >> shift) & 0x1;
		n->shift_register >>= 1;
		n->shift_register |= (b1 ^ b2) << 14;
	}
	else
		n->timer_value--;
}

static inline void noise_step_envelope(struct xnes_apu_noise_t * n)
{
	if(n->envelope_start)
	{
		n->envelope_volume = 15;
		n->envelope_value = n->envelope_period;
		n->envelope_start = 0;
	}
	else if(n->envelope_value > 0)
	{
		n->envelope_value--;
	}
	else
	{
		if(n->envelope_volume > 0)
			n->envelope_volume--;
		else if(n->envelope_loop)
			n->envelope_volume = 15;
		n->envelope_value = n->envelope_period;
	}
}

static inline void noise_step_length(struct xnes_apu_noise_t * n)
{
	if(n->length_enabled && (n->length_value > 0))
		n->length_value--;
}

static inline uint8_t noise_output(struct xnes_apu_noise_t * n)
{
	if(!n->enabled)
		return 0;
	if(n->length_value == 0)
		return 0;
	if(n->shift_register & 0x1)
		return 0;
	if(n->envelope_enabled)
		return n->envelope_volume;
	else
		return n->constant_volume;
}

static inline void dmc_write_control(struct xnes_apu_dmc_t * d, uint8_t value)
{
	d->irq = (value & 0x80) ? 1 : 0;
	d->loop = (value & 0x40) ? 1 : 0;
	d->tick_period = dmc_table[value & 0xf];
}

static inline void dmc_write_value(struct xnes_apu_dmc_t * d, uint8_t value)
{
	d->value = value & 0x7f;
}

static inline void dmc_write_address(struct xnes_apu_dmc_t * d, uint8_t value)
{
	d->sample_address = 0xc000 | (((uint16_t)value) << 6);
}

static inline void dmc_write_length(struct xnes_apu_dmc_t * d, uint8_t value)
{
	d->sample_length = (((uint16_t)value) << 4) | 0x1;
}

static inline void dmc_restart(struct xnes_apu_dmc_t * d)
{
	d->current_address = d->sample_address;
	d->current_length = d->sample_length;
}

static inline void dmc_step_reader(struct xnes_apu_dmc_t * d)
{
	if((d->current_length > 0) && (d->bit_count == 0))
	{
		d->ctx->cpu.stall += 4;
		d->shift_register = xnes_cpu_read8(&d->ctx->cpu, d->current_address);
		d->bit_count = 8;
		d->current_address++;
		if(d->current_address == 0)
			d->current_address = 0x8000;
		d->current_length--;
		if((d->current_length == 0) && d->loop)
			dmc_restart(d);
	}
}

static inline void dmc_step_shifter(struct xnes_apu_dmc_t * d)
{
	if(d->bit_count == 0)
		return;
	if(d->shift_register & 0x1)
	{
		if(d->value <= 125)
			d->value += 2;
	}
	else
	{
		if(d->value >= 2)
			d->value -= 2;
	}
	d->shift_register >>= 1;
	d->bit_count--;
}

static inline void dmc_step_timer(struct xnes_apu_dmc_t * d)
{
    if(!d->enabled)
		return;
	dmc_step_reader(d);
	if(d->tick_value == 0)
	{
		d->tick_value = d->tick_period;
		dmc_step_shifter(d);
	}
	else
		d->tick_value--;
}

static inline uint8_t dmc_output(struct xnes_apu_dmc_t * d)
{
	return d->value;
}

/*
 * y[n] = b0 * x[n] + b1 * x[n - 1] - a1 * y[n - 1]
 */
static inline float filter_step(struct xnes_apu_filter_t * f, float x)
{
	f->prevy = f->b0 * x + f->b1 * f->prevx - f->a1 * f->prevy;
	f->prevx = x;
	return f->prevy;
}

static inline void filter_low_pass(struct xnes_apu_filter_t * filter, float sample, float cutoff)
{
	float c = sample / 3.1415926 / cutoff;
	float a0i = 1 / (1 + c);
	filter->b0 = a0i;
	filter->b1 = a0i;
	filter->a1 = (1 - c) * a0i;
}

static inline void filter_high_pass(struct xnes_apu_filter_t * filter, float sample, float cutoff)
{
	float c = sample / 3.1415926 / cutoff;
	float a0i = 1 / (1 + c);
	filter->b0 = c * a0i;
	filter->b1 = -c * a0i;
	filter->a1 = (1 - c) * a0i;
}

static inline float filter_chain_step(struct xnes_apu_filter_t * f, uint8_t len, float x)
{
	if((f != NULL) && (len > 0))
	{
		for(int i = 0; i < len; i++)
			x = filter_step(&f[i], x);
	}
	return x;
}

static inline void step_envelope(struct xnes_apu_t * apu)
{
	pulse_step_envelope(&apu->pulse1);
	pulse_step_envelope(&apu->pulse2);
	triangle_step_counter(&apu->triangle);
	noise_step_envelope(&apu->noise);
}

static inline void step_sweep(struct xnes_apu_t * apu)
{
	pulse_step_sweep(&apu->pulse1);
	pulse_step_sweep(&apu->pulse2);
}

static inline void step_length(struct xnes_apu_t * apu)
{
	pulse_step_length(&apu->pulse1);
	pulse_step_length(&apu->pulse2);
	triangle_step_length(&apu->triangle);
	noise_step_length(&apu->noise);
}

static inline void fire_irq(struct xnes_apu_t * apu)
{
	if(apu->frame_irq)
		xnes_cpu_trigger_irq(&apu->ctx->cpu);
}

static inline float output(struct xnes_apu_t * apu)
{
	uint8_t p1 = pulse_output(&apu->pulse1);
	uint8_t p2 = pulse_output(&apu->pulse2);
	uint8_t t = triangle_output(&apu->triangle);
	uint8_t n = noise_output(&apu->noise);
	uint8_t d = dmc_output(&apu->dmc);
	return (float)pulse_table[p1 + p2] + (float)tnd_table[3 * t + 2 * n + d];
}

static inline void send_sample(struct xnes_apu_t * apu)
{
	if(apu->audio_callback)
		apu->audio_callback(apu->audio_ctx, filter_chain_step(apu->filter, 3, output(apu)));
}

static inline void step_frame_counter(struct xnes_apu_t * apu)
{
	switch(apu->frame_period)
	{
	case 4:
		apu->frame_value = (apu->frame_value + 1) % 4;
		switch(apu->frame_value)
		{
		case 0:
		case 2:
			step_envelope(apu);
			break;
		case 1:
			step_envelope(apu);
			step_sweep(apu);
			step_length(apu);
			break;
		case 3:
			step_envelope(apu);
			step_sweep(apu);
			step_length(apu);
			fire_irq(apu);
			break;
		default:
			break;
		}
		break;
	case 5:
		apu->frame_value = (apu->frame_value + 1) % 5;
		switch(apu->frame_value)
		{
		case 0:
		case 2:
			step_envelope(apu);
			break;
		case 1:
		case 3:
			step_envelope(apu);
			step_sweep(apu);
			step_length(apu);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

static inline void step_timer(struct xnes_apu_t * apu)
{
	if(!(apu->cycles & 0x1))
	{
		pulse_step_timer(&apu->pulse1);
		pulse_step_timer(&apu->pulse2);
		noise_step_timer(&apu->noise);
		dmc_step_timer(&(apu->dmc));
	}
	triangle_step_timer(&apu->triangle);
}

static inline void write_control(struct xnes_apu_t * apu, uint8_t value)
{
	apu->pulse1.enabled = (value & 0x1) ? 1 : 0;
	apu->pulse2.enabled = (value & 0x2) ? 1 : 0;
	apu->triangle.enabled = (value & 0x4) ? 1 : 0;
	apu->noise.enabled = (value & 0x8) ? 1 : 0;
	apu->dmc.enabled = (value & 0x10) ? 1 : 0;
	if(!apu->pulse1.enabled)
		apu->pulse1.length_value = 0;
	if(!apu->pulse2.enabled)
		apu->pulse2.length_value = 0;
	if(!apu->triangle.enabled)
		apu->triangle.length_value = 0;
	if(!apu->noise.enabled)
		apu->noise.length_value = 0;
	if(!apu->dmc.enabled)
		apu->dmc.current_length = 0;
	else
	{
		if(apu->dmc.current_length == 0)
			dmc_restart(&(apu->dmc));
	}
}

static inline void write_frame_counter(struct xnes_apu_t * apu, uint8_t value)
{
	apu->frame_period = 4 + ((value & 0x80) ? 1 : 0);
	apu->frame_irq = (value & 0x40) ? 0 : 1;
	if(apu->frame_period == 5)
	{
		step_envelope(apu);
		step_sweep(apu);
		step_length(apu);
	}
}

static inline uint8_t read_status(struct xnes_apu_t * apu)
{
	uint8_t val = 0;
	if(apu->pulse1.length_value > 0)
		val |= 0x1;
	if(apu->pulse2.length_value > 0)
		val |= 0x2;
	if(apu->triangle.length_value > 0)
		val |= 0x4;
	if(apu->noise.length_value > 0)
		val |= 0x8;
	if(apu->dmc.current_length > 0)
		val |= 0x10;
	return val;
}

void xnes_apu_init(struct xnes_apu_t * apu, struct xnes_ctx_t * ctx)
{
	apu->ctx = ctx;
	xnes_apu_reset(apu);
	xnes_apu_set_audio_callback(apu, NULL, NULL, 48000);
}

void xnes_apu_reset(struct xnes_apu_t * apu)
{
	memset(&apu->pulse1, 0, sizeof(struct xnes_apu_pulse_t));
	memset(&apu->pulse2, 0, sizeof(struct xnes_apu_pulse_t));
	memset(&apu->triangle, 0, sizeof(struct xnes_apu_triangle_t));
	memset(&apu->noise, 0, sizeof(struct xnes_apu_noise_t));
	memset(&apu->dmc, 0, sizeof(struct xnes_apu_dmc_t));
	apu->dmc.ctx = apu->ctx;
	apu->pulse1.channel = 1;
	apu->pulse2.channel = 2;
	apu->noise.shift_register = 1;
	apu->cycles = 0;
	apu->frame_period = 4;
	apu->frame_value = 0;
	apu->frame_irq = 0;
}

void xnes_apu_set_audio_callback(struct xnes_apu_t * apu, void * ctx, void (*cb)(void *, float), int rate)
{
	if(apu)
	{
		apu->audio_rate = rate;
		apu->audio_ctx = ctx;
		apu->audio_callback = cb;
		filter_high_pass(&apu->filter[0], apu->audio_rate, 90);
		filter_high_pass(&apu->filter[1], apu->audio_rate, 440);
		filter_low_pass(&apu->filter[2], apu->audio_rate, 14000);
	}
}

void xnes_apu_step(struct xnes_apu_t * apu)
{
	apu->cycles++;
	step_timer(apu);
	if((apu->cycles % (apu->ctx->cartridge->cpu_rate_adjusted / 240)) == 0)
		step_frame_counter(apu);
	if((apu->cycles % (apu->ctx->cartridge->cpu_rate_adjusted / apu->audio_rate)) == 0)
		send_sample(apu);
}

uint8_t xnes_apu_read_register(struct xnes_apu_t * apu, uint16_t addr)
{
	switch(addr)
	{
	case 0x4000: /* SQ1_VOL */
		break;
	case 0x4001: /* SQ1_SWEEP */
		break;
	case 0x4002: /* SQ1_LO */
		break;
	case 0x4003: /* SQ1_HI */
		break;
	case 0x4004: /* SQ2_VOL */
		break;
	case 0x4005: /* SQ2_SWEEP */
		break;
	case 0x4006: /* SQ2_LO */
		break;
	case 0x4007: /* SQ2_HI */
		break;
	case 0x4008: /* TRI_LINEAR */
		break;
	case 0x4009: /* UNUSED */
		break;
	case 0x400a: /* TRI_LO */
		break;
	case 0x400b: /* TRI_HI */
		break;
	case 0x400c: /* NOISE_VOL */
		break;
	case 0x400d: /* UNUSED */
		break;
	case 0x400e: /* NOISE_LO */
		break;
	case 0x400f: /* NOISE_HI */
		break;
	case 0x4010: /* DMC_FREQ */
		break;
	case 0x4011: /* DMC_RAW */
		break;
	case 0x4012: /* DMC_START */
		break;
	case 0x4013: /* DMC_LEN */
		break;
	case 0x4014: /* OAMDMA */
		break;
	case 0x4015: /* SND_CHN */
		return read_status(apu);
	default:
		break;
	}
	return 0;
}

void xnes_apu_write_register(struct xnes_apu_t * apu, uint16_t addr, uint8_t val)
{
	switch(addr)
	{
	case 0x4000: /* SQ1_VOL */
		pulse_write_control(&apu->pulse1, val);
		break;
	case 0x4001: /* SQ1_SWEEP */
		pulse_write_sweep(&apu->pulse1, val);
		break;
	case 0x4002: /* SQ1_LO */
		pulse_write_timer_low(&apu->pulse1, val);
		break;
	case 0x4003: /* SQ1_HI */
		pulse_write_timer_high(&apu->pulse1, val);
		break;
	case 0x4004: /* SQ2_VOL */
		pulse_write_control(&apu->pulse2, val);
		break;
	case 0x4005: /* SQ2_SWEEP */
		pulse_write_sweep(&apu->pulse2, val);
		break;
	case 0x4006: /* SQ2_LO */
		pulse_write_timer_low(&apu->pulse2, val);
		break;
	case 0x4007: /* SQ2_HI */
		pulse_write_timer_high(&apu->pulse2, val);
		break;
	case 0x4008: /* TRI_LINEAR */
		triangle_write_control(&apu->triangle, val);
		break;
	case 0x4009: /* UNUSED */
		break;
	case 0x400a: /* TRI_LO */
		triangle_write_timer_low(&apu->triangle, val);
		break;
	case 0x400b: /* TRI_HI */
		triangle_write_timer_high(&apu->triangle, val);
		break;
	case 0x400c: /* NOISE_VOL */
		noise_write_control(&apu->noise, val);
		break;
	case 0x400d: /* UNUSED */
		break;
	case 0x400e: /* NOISE_LO */
		noise_write_period(&apu->noise, val);
		break;
	case 0x400f: /* NOISE_HI */
		noise_write_length(&apu->noise, val);
		break;
	case 0x4010: /* DMC_FREQ */
		dmc_write_control(&apu->dmc, val);
		break;
	case 0x4011: /* DMC_RAW */
		dmc_write_value(&apu->dmc, val);
		break;
	case 0x4012: /* DMC_START */
		dmc_write_address(&apu->dmc, val);
		break;
	case 0x4013: /* DMC_LEN */
		dmc_write_length(&apu->dmc, val);
		break;
	case 0x4014: /* OAMDMA */
		break;
	case 0x4015: /* SND_CHN */
		write_control(apu, val);
		break;
	case 0x4017: /* APU_FRAME */
		write_frame_counter(apu, val);
		break;
	default:
		break;
	}
}
