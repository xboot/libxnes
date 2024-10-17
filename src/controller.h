#ifndef __XNES_CONTROLLER_H__
#define __XNES_CONTROLLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnesconf.h>

struct xnes_ctx_t;

enum {
	XNES_JOYSTICK_A			= (1 << 7),
	XNES_JOYSTICK_B			= (1 << 6),
	XNES_JOYSTICK_SELECT	= (1 << 5),
	XNES_JOYSTICK_START		= (1 << 4),
	XNES_JOYSTICK_UP		= (1 << 3),
	XNES_JOYSTICK_DOWN		= (1 << 2),
	XNES_JOYSTICK_LEFT		= (1 << 1),
	XNES_JOYSTICK_RIGHT		= (1 << 0),
};

struct xnes_controller_t {
	struct xnes_ctx_t * ctx;

	uint8_t strobe;
	uint8_t p1;
	uint8_t p1_turbo;
	uint8_t p1_index;
	uint8_t p2;
	uint8_t p2_turbo;
	uint8_t p2_index;
	uint8_t x, y;
	uint8_t trigger;
};

void xnes_controller_init(struct xnes_controller_t * ctl, struct xnes_ctx_t * ctx);
void xnes_controller_reset(struct xnes_controller_t * ctl);
uint8_t xnes_controller_read_register(struct xnes_controller_t * ctl, uint16_t addr);
void xnes_controller_write_register(struct xnes_controller_t * ctl, uint16_t addr, uint8_t val);

void xnes_controller_joystick_p1(struct xnes_controller_t * ctl, uint8_t down, uint8_t up);
void xnes_controller_joystick_p2(struct xnes_controller_t * ctl, uint8_t down, uint8_t up);
void xnes_controller_joystick_p1_turbo(struct xnes_controller_t * ctl, uint8_t down, uint8_t up);
void xnes_controller_joystick_p2_turbo(struct xnes_controller_t * ctl, uint8_t down, uint8_t up);
void xnes_controller_zapper(struct xnes_controller_t * ctl, uint8_t x, uint8_t y, uint8_t trigger);

#ifdef __cplusplus
}
#endif

#endif /* __XNES_CONTROLLER_H__ */
