#include <debugger.h>
#include <context.h>
#include <command.h>

static void usage(void)
{
	shell_printf("usage:\r\n");
	shell_printf("    breakpoint <address>\r\n");
}

static int do_breakpoint(struct xnes_ctx_t * ctx, int argc, char ** argv)
{
	if(argc <= 1)
		shell_printf("BP: %04X\r\n", debugger_get_breakpoint(ctx));
	else
		debugger_set_breakpoint(ctx, (uint16_t)strtoul(argv[1], NULL, 0));
	return 0;
}

struct command_t cmd_breakpoint = {
	.name	= "breakpoint",
	.desc	= "set breakpoint address",
	.usage	= usage,
	.exec	= do_breakpoint,
};
