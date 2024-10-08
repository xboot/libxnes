#ifndef __XNES_PPU_H__
#define __XNES_PPU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnescore.h>

void xnes_ppu_init(struct xnes_ppu_t * ppu, struct xnes_ctx_t * ctx);
void xnes_ppu_reset(struct xnes_ppu_t * ppu);
void xnes_ppu_step(struct xnes_ppu_t * ppu);
uint8_t xnes_ppu_read_register(struct xnes_ppu_t * ppu, uint16_t addr);
void xnes_ppu_write_register(struct xnes_ppu_t * ppu, uint16_t addr, uint8_t val);
void xnes_ppu_set_palette(struct xnes_ppu_t * ppu, uint32_t * palette);
uint8_t xnes_ppu_is_white_pixel(struct xnes_ppu_t * ppu, uint8_t x, uint8_t y);

#ifdef __cplusplus
}
#endif

#endif /* __XNES_PPU_H__ */
