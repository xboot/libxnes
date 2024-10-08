#ifndef __XNES_CONTROLLER_H__
#define __XNES_CONTROLLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnescore.h>

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
