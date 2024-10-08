#ifndef __XNES_APU_H__
#define __XNES_APU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnescore.h>

void xnes_apu_init(struct xnes_apu_t * apu, struct xnes_ctx_t * ctx);
void xnes_apu_reset(struct xnes_apu_t * apu);
void xnes_apu_step(struct xnes_apu_t * apu);
uint8_t xnes_apu_read_register(struct xnes_apu_t * apu, uint16_t addr);
void xnes_apu_write_register(struct xnes_apu_t * apu, uint16_t addr, uint8_t val);
void xnes_apu_set_audio_callback(struct xnes_apu_t * apu, void * ctx, void (*cb)(void *, float), int rate);

#ifdef __cplusplus
}
#endif

#endif /* __XNES_APU_H__ */
