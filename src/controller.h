#ifndef __XNES_CONTROLLER_H__
#define __XNES_CONTROLLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnesconf.h>

struct xnes_ctx_t;

enum {
	XNES_CONTROLLER_TURBO_SPEED_FAST	= (1 << 1),
	XNES_CONTROLLER_TURBO_SPEED_NORMAL	= (1 << 2),
	XNES_CONTROLLER_TURBO_SPEED_SLOW	= (1 << 3),
};

enum {
	XNES_JOYSTICK_A						= (1 << 7),
	XNES_JOYSTICK_B						= (1 << 6),
	XNES_JOYSTICK_SELECT				= (1 << 5),
	XNES_JOYSTICK_START					= (1 << 4),
	XNES_JOYSTICK_UP					= (1 << 3),
	XNES_JOYSTICK_DOWN					= (1 << 2),
	XNES_JOYSTICK_LEFT					= (1 << 1),
	XNES_JOYSTICK_RIGHT					= (1 << 0),
};

struct xnes_controller_t {
	struct xnes_ctx_t * ctx;

	uint8_t turbo_count;
	uint8_t turbo_speed;
	uint8_t latch;

	struct {
		struct {
			uint8_t key;
			uint8_t key_turbo;
			uint8_t key_index;
		} p1;
		struct {
			uint8_t key;
			uint8_t key_turbo;
			uint8_t key_index;
		} p2;
	} joystick;

	struct {
		uint8_t x, y;
		uint8_t trigger;
	} zapper;
};

void xnes_controller_init(struct xnes_controller_t * ctl, struct xnes_ctx_t * ctx);
void xnes_controller_reset(struct xnes_controller_t * ctl);
void xnes_controller_set_turbo_speed(struct xnes_controller_t * ctl, uint8_t speed);
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
