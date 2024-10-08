#ifndef __XNES_CPU_H__
#define __XNES_CPU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnescore.h>

void xnes_cpu_init(struct xnes_cpu_t * cpu, struct xnes_ctx_t * ctx);
void xnes_cpu_reset(struct xnes_cpu_t * cpu);
uint8_t xnes_cpu_read8(struct xnes_cpu_t * cpu, uint16_t addr);
void xnes_cpu_write8(struct xnes_cpu_t * cpu, uint16_t addr, uint8_t val);
void xnes_cpu_trigger_nmi(struct xnes_cpu_t * cpu);
void xnes_cpu_trigger_irq(struct xnes_cpu_t * cpu);
int xnes_cpu_step(struct xnes_cpu_t * cpu);
void xnes_cpu_dump(struct xnes_cpu_t * cpu);

#ifdef __cplusplus
}
#endif

#endif /* __XNES_CPU_H__ */
