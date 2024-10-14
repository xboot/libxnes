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

#include <SDL.h>
#include <windows.h>
#include <xnes.h>

#define SDL_SCREEN_SCALE	(4)

struct window_context_t {
	SDL_Window * window;
	SDL_Surface * screen;
	SDL_Surface * surface;
	SDL_Renderer * renderer;
	SDL_Joystick * joy[2];
	SDL_AudioDeviceID audio;
	SDL_AudioSpec wantspec, havespec;
	int width;
	int height;

	struct xnes_ctx_t * nes;
	struct xnes_state_t * state;
	int rewind;
	uint64_t timestamp;
	uint64_t elapsed;
};

static uint64_t ktime_get(void)
{
	return SDL_GetPerformanceCounter() * 1000000000.0 / SDL_GetPerformanceFrequency();
}

static struct window_context_t * window_context_alloc(void)
{
	struct window_context_t * wctx;
	Uint32 r, g, b, a;
	int bpp;

	wctx = malloc(sizeof(struct window_context_t));
	if(!wctx)
		return NULL;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK);
	SDL_EnableScreenSaver();
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
	SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");

	wctx->window = SDL_CreateWindow("XNES - The Nintendo Entertainment System Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256 * SDL_SCREEN_SCALE, 240 * SDL_SCREEN_SCALE, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);
	wctx->screen = SDL_GetWindowSurface(wctx->window);
	SDL_GetWindowSize(wctx->window, &wctx->width, &wctx->height);

	SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888, &bpp, &r, &g, &b, &a);
	wctx->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 256 * SDL_SCREEN_SCALE, 240 * SDL_SCREEN_SCALE, bpp, r, g, b, a);
	wctx->renderer = SDL_CreateSoftwareRenderer(wctx->surface);
	wctx->joy[0] = NULL;
	wctx->joy[1] = NULL;

	wctx->wantspec.freq = 48000;
	wctx->wantspec.format = AUDIO_F32SYS;
	wctx->wantspec.channels = 2;
	wctx->wantspec.samples = 2048;
	wctx->wantspec.callback = NULL;
	wctx->audio = SDL_OpenAudioDevice(0, 0, &wctx->wantspec, &wctx->havespec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_SAMPLES_CHANGE);
	SDL_ClearQueuedAudio(wctx->audio);
	SDL_PauseAudioDevice(wctx->audio, 0);

	wctx->nes = NULL;
	wctx->state = NULL;
	wctx->rewind = 0;
	wctx->timestamp = ktime_get();
	wctx->elapsed = 0;

	return wctx;
}

static void window_context_free(struct window_context_t * wctx)
{
	if(wctx)
	{
		if(wctx->nes)
			xnes_ctx_free(wctx->nes);
		if(wctx->state)
			xnes_state_free(wctx->state);
		if(wctx->joy[0])
		{
			SDL_JoystickClose(wctx->joy[0]);
			wctx->joy[0] = NULL;
		}
		if(wctx->joy[1])
		{
			SDL_JoystickClose(wctx->joy[1]);
			wctx->joy[1] = NULL;
		}
		SDL_CloseAudio();
		if(wctx->screen)
			SDL_FreeSurface(wctx->screen);
		if(wctx->surface)
			SDL_FreeSurface(wctx->surface);
		if(wctx->renderer)
			SDL_DestroyRenderer(wctx->renderer);
		if(wctx->window)
			SDL_DestroyWindow(wctx->window);
		free(wctx);
	}
}

static void window_context_screen_refresh(struct window_context_t * wctx)
{
	uint32_t * fb = wctx->surface->pixels;
	for(int y = 0; y < 240; y++)
	{
		for(int x = 0; x < 256; x++)
		{
			uint32_t c = wctx->nes->ppu.front[(y * 256) + x];
			int p = ((y * SDL_SCREEN_SCALE) * (256 * SDL_SCREEN_SCALE)) + x * SDL_SCREEN_SCALE;
			for(int j = 0; j < SDL_SCREEN_SCALE; j++)
			{
				for(int i = 0; i < SDL_SCREEN_SCALE; i++)
					fb[p + i] = c;
				p += (256 * SDL_SCREEN_SCALE);
			}
		}
	}
	SDL_BlitSurface(wctx->surface, NULL, wctx->screen, NULL);
	SDL_UpdateWindowSurface(wctx->window);
}

static void window_context_update(struct window_context_t * wctx)
{
	if(wctx->nes)
	{
		if(wctx->rewind)
		{
			if((ktime_get() - wctx->timestamp) >= wctx->elapsed)
			{
				wctx->timestamp = ktime_get();
				wctx->elapsed = 16666666;
				xnes_state_pop(wctx->state);
				window_context_screen_refresh(wctx);
			}
			else
				SDL_Delay(1);
		}
		else
		{
			if((ktime_get() - wctx->timestamp) >= wctx->elapsed)
			{
				wctx->timestamp = ktime_get();
				wctx->elapsed = xnes_step_frame(wctx->nes);
				window_context_screen_refresh(wctx);
				xnes_state_push(wctx->state);
			}
			else
				SDL_Delay(1);
		}
	}
	else
		SDL_Delay(1);
}

static void * file_load(const char * filename, uint64_t * len)
{
	int l = strlen(filename);
	int wl = MultiByteToWideChar(CP_UTF8, 0, filename, l, NULL, 0);
	wchar_t wfilename[wl + 1];
	MultiByteToWideChar(CP_UTF8, 0, filename, l, wfilename, wl);
	wfilename[wl] = L'\0';

	FILE * in = _wfopen(wfilename, L"rb");
	if(in)
	{
		uint64_t offset = 0, bufsize = 8192;
		char * buf = malloc(bufsize);
		while(1)
		{
			uint64_t len = bufsize - offset;
			uint64_t n = fread(buf + offset, 1, len, in);
			offset += n;
			if(n < len)
				break;
			bufsize *= 2;
			buf = realloc(buf, bufsize);
		}
		if(len)
			*len = offset;
		fclose(in);
		return buf;
	}
	return NULL;
}

static void window_audio_callback(void * ctx, float v)
{
	struct window_context_t * wctx = (struct window_context_t *)ctx;
	SDL_QueueAudio(wctx->audio, &v, sizeof(float));
	SDL_QueueAudio(wctx->audio, &v, sizeof(float));
}

static void window_context_reload(struct window_context_t * wctx, const char * filename)
{
	void * buf;
	uint64_t len;

	if(wctx)
	{
		if(wctx->nes)
		{
			xnes_ctx_free(wctx->nes);
			wctx->nes = NULL;
		}
		buf = file_load(filename, &len);
		if(buf)
		{
			wctx->nes = xnes_ctx_alloc(buf, len);
			if(wctx->nes)
			{
				if(wctx->state)
					xnes_state_free(wctx->state);
				wctx->state = xnes_state_alloc(wctx->nes, 60 * 30);

				SDL_ClearQueuedAudio(wctx->audio);
				xnes_set_audio(wctx->nes, wctx, window_audio_callback, wctx->havespec.freq);
			}
			free(buf);
		}
	}
}

int main(int argc, char * argv[])
{
	struct window_context_t * wctx = window_context_alloc();
	SDL_Event e;
	int done = 0;

	if(argc > 1)
		window_context_reload(wctx, argv[1]);
	while(!done)
	{
		if(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
			case SDL_QUIT:
				done = 1;
				break;

			case SDL_DROPFILE:
				window_context_reload(wctx, e.drop.file);
				SDL_free(e.drop.file);
				break;

			case SDL_JOYDEVICEADDED:
				for(int i = 0; i < SDL_NumJoysticks(); i++)
				{
					if(!(wctx->joy[0] && SDL_JoystickGetAttached(wctx->joy[0])) || !(wctx->joy[1] && SDL_JoystickGetAttached(wctx->joy[1])))
					{
						SDL_Joystick * joy = SDL_JoystickOpen(i);
						if(joy)
						{
							if(!(wctx->joy[0] && SDL_JoystickGetAttached(wctx->joy[0])))
								wctx->joy[0] = joy;
							else if(!(wctx->joy[1] && SDL_JoystickGetAttached(wctx->joy[1])))
								wctx->joy[1] = joy;
						}
					}
					else
					{
						break;
					}
				}
				break;

			case SDL_JOYDEVICEREMOVED:
				if(wctx->joy[0] && !SDL_JoystickGetAttached(wctx->joy[0]))
				{
					SDL_JoystickClose(wctx->joy[0]);
					wctx->joy[0] = NULL;
				}
				if(wctx->joy[1] && !SDL_JoystickGetAttached(wctx->joy[1]))
				{
					SDL_JoystickClose(wctx->joy[1]);
					wctx->joy[1] = NULL;
				}
				break;

			default:
				break;
			}
			if(wctx->nes)
			{
				switch(e.type)
				{
				case SDL_MOUSEBUTTONDOWN:
					if(e.button.button == SDL_BUTTON_LEFT)
						xnes_controller_zapper(&wctx->nes->ctl, e.button.x / SDL_SCREEN_SCALE, e.button.y / SDL_SCREEN_SCALE, 1);
					break;

				case SDL_MOUSEBUTTONUP:
					if(e.button.button == SDL_BUTTON_LEFT)
						xnes_controller_zapper(&wctx->nes->ctl, e.button.x / SDL_SCREEN_SCALE, e.button.y / SDL_SCREEN_SCALE, 0);
					break;

				case SDL_KEYDOWN:
					switch(e.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						xnes_reset(wctx->nes);
						break;

					case SDLK_F1:
						xnes_set_speed(wctx->nes, 0.5);
						break;

					case SDLK_F2:
						xnes_set_speed(wctx->nes, 2.0);
						break;

					case SDLK_r:
						wctx->rewind = 1;
						break;

					case SDLK_w:
						xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_UP, 0);
						break;
					case SDLK_s:
						xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_DOWN, 0);
						break;
					case SDLK_a:
						xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_LEFT, 0);
						break;
					case SDLK_d:
						xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_RIGHT, 0);
						break;
					case SDLK_k:
						xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_A, 0);
						break;
					case SDLK_j:
						xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_B, 0);
						break;
					case SDLK_SPACE:
						xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_SELECT, 0);
						break;
					case SDLK_RETURN:
						xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_START, 0);
						break;

					case SDLK_UP:
						xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_UP, 0);
						break;
					case SDLK_DOWN:
						xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_DOWN, 0);
						break;
					case SDLK_LEFT:
						xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_LEFT, 0);
						break;
					case SDLK_RIGHT:
						xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_RIGHT, 0);
						break;
					case SDLK_KP_6:
						xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_A, 0);
						break;
					case SDLK_KP_5:
						xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_B, 0);
						break;
					case SDLK_KP_8:
						xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_SELECT, 0);
						break;
					case SDLK_KP_9:
						xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_START, 0);
						break;

					default:
						break;
					}
					break;

				case SDL_KEYUP:
					switch(e.key.keysym.sym)
					{
					case SDLK_F1:
						xnes_set_speed(wctx->nes, 1.0);
						break;

					case SDLK_F2:
						xnes_set_speed(wctx->nes, 1.0);
						break;

					case SDLK_r:
						wctx->rewind = 0;
						break;

					case SDLK_w:
						xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_UP);
						break;
					case SDLK_s:
						xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_DOWN);
						break;
					case SDLK_a:
						xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_LEFT);
						break;
					case SDLK_d:
						xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_RIGHT);
						break;
					case SDLK_k:
						xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_A);
						break;
					case SDLK_j:
						xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_B);
						break;
					case SDLK_SPACE:
						xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_SELECT);
						break;
					case SDLK_RETURN:
						xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_START);
						break;

					case SDLK_UP:
						xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_UP);
						break;
					case SDLK_DOWN:
						xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_DOWN);
						break;
					case SDLK_LEFT:
						xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_LEFT);
						break;
					case SDLK_RIGHT:
						xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_RIGHT);
						break;
					case SDLK_KP_6:
						xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_A);
						break;
					case SDLK_KP_5:
						xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_B);
						break;
					case SDLK_KP_8:
						xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_SELECT);
						break;
					case SDLK_KP_9:
						xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_START);
						break;

					default:
						break;
					}
					break;

				case SDL_JOYAXISMOTION:
					if(e.jaxis.which == SDL_JoystickInstanceID(wctx->joy[0]))
					{
						switch(e.jaxis.axis)
						{
						case 0:
							if(e.jaxis.value < -3200)
								xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_LEFT, 0);
							else if(e.jaxis.value > 3200)
								xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_RIGHT, 0);
							else
								xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_LEFT | XNES_JOYSTICK_RIGHT);
							break;
						case 1:
							if(e.jaxis.value < -3200)
								xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_UP, 0);
							else if(e.jaxis.value > 3200)
								xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_DOWN, 0);
							else
								xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_UP | XNES_JOYSTICK_DOWN);
							break;
						default:
							break;
						}
					}
					else if(e.jaxis.which == SDL_JoystickInstanceID(wctx->joy[1]))
					{
						switch(e.jaxis.axis)
						{
						case 0:
							if(e.jaxis.value < -3200)
								xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_LEFT, 0);
							else if(e.jaxis.value > 3200)
								xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_RIGHT, 0);
							else
								xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_LEFT | XNES_JOYSTICK_RIGHT);
							break;
						case 1:
							if(e.jaxis.value < -3200)
								xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_UP, 0);
							else if(e.jaxis.value > 3200)
								xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_DOWN, 0);
							else
								xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_UP | XNES_JOYSTICK_DOWN);
							break;
						default:
							break;
						}
					}
					break;

				case SDL_JOYBUTTONDOWN:
					if(e.jaxis.which == SDL_JoystickInstanceID(wctx->joy[0]))
					{
						switch(e.jbutton.button)
						{
						case 0: /* X */
							xnes_controller_joystick_p1_turbo(&wctx->nes->ctl, XNES_JOYSTICK_A, 0);
							break;
						case 1: /* A */
							xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_A, 0);
							break;
						case 2: /* B */
							xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_B, 0);
							break;
						case 3: /* Y */
							xnes_controller_joystick_p1_turbo(&wctx->nes->ctl, XNES_JOYSTICK_B, 0);
							break;
						case 4: /* L */
							wctx->rewind = 1;
							break;
						case 5: /* R */
							xnes_set_speed(wctx->nes, 2.0);
							break;
						case 8: /* Select */
							xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_SELECT, 0);
							break;
						case 9: /* Start */
							xnes_controller_joystick_p1(&wctx->nes->ctl, XNES_JOYSTICK_START, 0);
							break;
						default:
							break;
						}
					}
					else if(e.jaxis.which == SDL_JoystickInstanceID(wctx->joy[1]))
					{
						switch(e.jbutton.button)
						{
						case 0: /* X */
							xnes_controller_joystick_p2_turbo(&wctx->nes->ctl, XNES_JOYSTICK_A, 0);
							break;
						case 1: /* A */
							xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_A, 0);
							break;
						case 2: /* B */
							xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_B, 0);
							break;
						case 3: /* Y */
							xnes_controller_joystick_p2_turbo(&wctx->nes->ctl, XNES_JOYSTICK_B, 0);
							break;
						case 4: /* L */
							wctx->rewind = 1;
							break;
						case 5: /* R */
							xnes_set_speed(wctx->nes, 2.0);
							break;
						case 8: /* Select */
							xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_SELECT, 0);
							break;
						case 9: /* Start */
							xnes_controller_joystick_p2(&wctx->nes->ctl, XNES_JOYSTICK_START, 0);
							break;
						default:
							break;
						}
					}
					break;

				case SDL_JOYBUTTONUP:
					if(e.jaxis.which == SDL_JoystickInstanceID(wctx->joy[0]))
					{
						switch(e.jbutton.button)
						{
						case 0: /* X */
							xnes_controller_joystick_p1_turbo(&wctx->nes->ctl, 0, XNES_JOYSTICK_A);
							break;
						case 1: /* A */
							xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_A);
							break;
						case 2: /* B */
							xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_B);
							break;
						case 3: /* Y */
							xnes_controller_joystick_p1_turbo(&wctx->nes->ctl, 0, XNES_JOYSTICK_B);
							break;
						case 4: /* L */
							wctx->rewind = 0;
							break;
						case 5: /* R */
							xnes_set_speed(wctx->nes, 1.0);
							break;
						case 8: /* Select */
							xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_SELECT);
							break;
						case 9: /* Start */
							xnes_controller_joystick_p1(&wctx->nes->ctl, 0, XNES_JOYSTICK_START);
							break;
						default:
							break;
						}
					}
					else if(e.jaxis.which == SDL_JoystickInstanceID(wctx->joy[1]))
					{
						switch(e.jbutton.button)
						{
						case 0: /* X */
							xnes_controller_joystick_p2_turbo(&wctx->nes->ctl, 0, XNES_JOYSTICK_A);
							break;
						case 1: /* A */
							xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_A);
							break;
						case 2: /* B */
							xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_B);
							break;
						case 3: /* Y */
							xnes_controller_joystick_p2_turbo(&wctx->nes->ctl, 0, XNES_JOYSTICK_B);
							break;
						case 4: /* L */
							wctx->rewind = 0;
							break;
						case 5: /* R */
							xnes_set_speed(wctx->nes, 1.0);
							break;
						case 8: /* Select */
							xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_SELECT);
							break;
						case 9: /* Start */
							xnes_controller_joystick_p2(&wctx->nes->ctl, 0, XNES_JOYSTICK_START);
							break;
						default:
							break;
						}
					}
					break;

				default:
					break;
				}
			}
		}
		window_context_update(wctx);
	}
	window_context_free(wctx);
	SDL_Quit();

	return 0;
}
