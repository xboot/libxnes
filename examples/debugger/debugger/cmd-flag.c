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
	uint8_t status = 0;

	status |= cpu->c << 0;
	status |= cpu->z << 1;
	status |= cpu->i << 2;
	status |= cpu->d << 3;
	status |= cpu->b << 4;
	status |= cpu->u << 5;
	status |= cpu->v << 6;
	status |= cpu->n << 7;
	shell_printf("PC:%04X A:%02X X:%02X Y:%02X P:%02X[%s%s%s%s%s%s%s%s] SP:%02X CYC:%ld\r\n",
		cpu->pc,
		cpu->a,
		cpu->x,
		cpu->y,
		status,
		cpu->c ? "C" : "-",
		cpu->z ? "Z" : "-",
		cpu->i ? "I" : "-",
		cpu->d ? "D" : "-",
		cpu->b ? "B" : "-",
		cpu->u ? "U" : "-",
		cpu->v ? "V" : "-",
		cpu->n ? "N" : "-",
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
