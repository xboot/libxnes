#ifndef __XNES_MAPPER_H__
#define __XNES_MAPPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnescore.h>

void xnes_mapper0_init(struct xnes_cartridge_t * c);
void xnes_mapper1_init(struct xnes_cartridge_t * c);
void xnes_mapper2_init(struct xnes_cartridge_t * c);
void xnes_mapper3_init(struct xnes_cartridge_t * c);
void xnes_mapper4_init(struct xnes_cartridge_t * c);
void xnes_mapper7_init(struct xnes_cartridge_t * c);
void xnes_mapper66_init(struct xnes_cartridge_t * c);
void xnes_mapper225_init(struct xnes_cartridge_t * c);

#ifdef __cplusplus
}
#endif

#endif /* __XNES_MAPPER_H__ */
