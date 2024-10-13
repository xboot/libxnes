#include <debugger.h>
#include <context.h>
#include <command.h>

static void usage(void)
{
	shell_printf("usage:\r\n");
	shell_printf("    flag\r\n");
}

static int do_flag(struct xnes_ctx_t * ctx, int argc, char ** argv)
{
	struct xnes_cpu_t * cpu = &ctx->cpu;

	shell_printf("PC:%04X A:%02X X:%02X Y:%02X P:%02X[%s%s%s%s%s%s%s%s] SP:%02X CYC:%ld\r\n",
		cpu->pc,
		cpu->a,
		cpu->x,
		cpu->y,
		cpu->p,
		cpu->p & XNES_CPU_P_C ? "C" : "-",
		cpu->p & XNES_CPU_P_Z ? "Z" : "-",
		cpu->p & XNES_CPU_P_I ? "I" : "-",
		cpu->p & XNES_CPU_P_D ? "D" : "-",
		cpu->p & XNES_CPU_P_B ? "B" : "-",
		cpu->p & XNES_CPU_P_U ? "U" : "-",
		cpu->p & XNES_CPU_P_V ? "V" : "-",
		cpu->p & XNES_CPU_P_N ? "N" : "-",
		cpu->sp,
		cpu->cycles
	);
	return 0;
}

struct command_t cmd_flag = {
	.name	= "flag",
	.desc	= "show the cpu current flag",
	.usage	= usage,
	.exec	= do_flag,
};
