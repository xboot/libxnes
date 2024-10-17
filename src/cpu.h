#ifndef __XNES_CPU_H__
#define __XNES_CPU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xnesconf.h>

struct xnes_ctx_t;

enum {
	XNES_CPU_P_C	= (1 << 0),	/* carry */
	XNES_CPU_P_Z	= (1 << 1),	/* zero */
	XNES_CPU_P_I	= (1 << 2),	/* interrupt */
	XNES_CPU_P_D	= (1 << 3),	/* decimal */
	XNES_CPU_P_B	= (1 << 4),	/* break */
	XNES_CPU_P_U	= (1 << 5),	/* unused */
	XNES_CPU_P_V	= (1 << 6),	/* overflow */
	XNES_CPU_P_N	= (1 << 7),	/* negative */
};

struct xnes_cpu_t {
	struct xnes_ctx_t * ctx;

	uint8_t ram[2048];
	uint64_t cycles;
	uint32_t stall;
	uint16_t pc;	/* program counter */
	uint8_t sp;		/* stack pointer */
	uint8_t a;		/* accumulator */
	uint8_t x;		/* index register x */
	uint8_t y;		/* index register y */
	uint8_t p;		/* processor status */
	uint8_t interrupt;

	/*
	 * debugger callback, cpu will be paused when return true.
	 */
	int (*debugger)(struct xnes_ctx_t *);
};

void xnes_cpu_init(struct xnes_cpu_t * cpu, struct xnes_ctx_t * ctx);
void xnes_cpu_reset(struct xnes_cpu_t * cpu);
uint8_t xnes_cpu_read8(struct xnes_cpu_t * cpu, uint16_t addr);
void xnes_cpu_write8(struct xnes_cpu_t * cpu, uint16_t addr, uint8_t val);
void xnes_cpu_trigger_nmi(struct xnes_cpu_t * cpu);
void xnes_cpu_trigger_irq(struct xnes_cpu_t * cpu);
int xnes_cpu_step(struct xnes_cpu_t * cpu);

#ifdef __cplusplus
}
#endif

#endif /* __XNES_CPU_H__ */
