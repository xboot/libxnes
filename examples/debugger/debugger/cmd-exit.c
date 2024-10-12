#include <debugger.h>
#include <context.h>
#include <command.h>

static void usage(void)
{
	shell_printf("usage:\r\n");
	shell_printf("    exit\r\n");
}

static int do_exit(struct xnes_ctx_t * ctx, int argc, char ** argv)
{
	debugger_exit(ctx);
	exit(0);
	return 0;
}

struct command_t cmd_exit = {
	.name	= "exit",
	.desc	= "exit the xstar program",
	.usage	= usage,
	.exec	= do_exit,
};
