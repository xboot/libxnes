#ifndef __XNES_DMA_H__
#define __XNES_DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnescore.h>

void xnes_dma_init(struct xnes_dma_t * dma, struct xnes_ctx_t * ctx);
void xnes_dma_reset(struct xnes_dma_t * dma);
uint8_t xnes_dma_read_register(struct xnes_dma_t * dma, uint16_t addr);
void xnes_dma_write_register(struct xnes_dma_t * dma, uint16_t addr, uint8_t val);

#ifdef __cplusplus
}
#endif

#endif /* __XNES_DMA_H__ */
