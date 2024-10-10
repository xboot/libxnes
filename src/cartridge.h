#ifndef __XNES_CARTRIDGE_H__
#define __XNES_CARTRIDGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnescore.h>

struct xnes_cartridge_t * xnes_cartridge_alloc(const void * buf, size_t len, struct xnes_ctx_t * ctx);
void xnes_cartridge_free(struct xnes_cartridge_t * c);
uint8_t xnes_cartridge_mapper_cpu_read(struct xnes_ctx_t * ctx, uint16_t addr);
void xnes_cartridge_mapper_cpu_write(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val);
uint8_t xnes_cartridge_mapper_ppu_read(struct xnes_ctx_t * ctx, uint16_t addr);
void xnes_cartridge_mapper_ppu_write(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val);
void xnes_cartridge_mapper_apu_step(struct xnes_ctx_t * ctx);
void xnes_cartridge_mapper_ppu_step(struct xnes_ctx_t * ctx);

#ifdef __cplusplus
}
#endif

#endif /* __XNES_CARTRIDGE_H__ */
