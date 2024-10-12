#include <debugger.h>
#include <context.h>
#include <command.h>

static void usage(void)
{
	shell_printf("usage:\r\n");
	shell_printf("    pause\r\n");
}

static int do_pause(struct xnes_ctx_t * ctx, int argc, char ** argv)
{
	debugger_step(ctx, 0);
	return 0;
}

struct command_t cmd_pause = {
	.name	= "pause",
	.desc	= "pause the nes program",
	.usage	= usage,
	.exec	= do_pause,
};
