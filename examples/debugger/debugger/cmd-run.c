#include <debugger.h>
#include <context.h>
#include <command.h>

static void usage(void)
{
	shell_printf("usage:\r\n");
	shell_printf("    run\r\n");
}

static int do_run(struct xnes_ctx_t * ctx, int argc, char ** argv)
{
	debugger_run(ctx);
	return 0;
}

struct command_t cmd_run = {
	.name	= "run",
	.desc	= "run the nes program with full speed",
	.usage	= usage,
	.exec	= do_run,
};
