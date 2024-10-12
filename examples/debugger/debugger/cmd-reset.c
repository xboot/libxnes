#include <debugger.h>
#include <context.h>
#include <command.h>

static void usage(void)
{
	shell_printf("usage:\r\n");
	shell_printf("    reset\r\n");
}

static int do_reset(struct xnes_ctx_t * ctx, int argc, char ** argv)
{
	xnes_reset(ctx);
	return 0;
}

struct command_t cmd_reset = {
	.name	= "reset",
	.desc	= "reset the emulator",
	.usage	= usage,
	.exec	= do_reset,
};
