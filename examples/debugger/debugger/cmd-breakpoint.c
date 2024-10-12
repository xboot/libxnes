#include <debugger.h>
#include <context.h>
#include <command.h>

static void usage(void)
{
	shell_printf("usage:\r\n");
	shell_printf("    breakpoint [address]\r\n");
}

static int do_breakpoint(struct xnes_ctx_t * ctx, int argc, char ** argv)
{
	uint16_t addr = 0;

	if(argc > 1)
		addr = strtoul(argv[1], NULL, 0);
	debugger_breakpoint(ctx, addr);

	return 0;
}

struct command_t cmd_breakpoint = {
	.name	= "breakpoint",
	.desc	= "set breakpoint address",
	.usage	= usage,
	.exec	= do_breakpoint,
};
