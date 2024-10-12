#include <debugger.h>
#include <context.h>
#include <command.h>

static void usage(void)
{
	shell_printf("usage:\r\n");
	shell_printf("    step [count]\r\n");
}

static int do_step(struct xnes_ctx_t * ctx, int argc, char ** argv)
{
	int count = 1;

	if(argc > 1)
		count = strtoul(argv[1], NULL, 0);
	debugger_step(ctx, count);

	return 0;
}

struct command_t cmd_step = {
	.name	= "step",
	.desc	= "step the nes program",
	.usage	= usage,
	.exec	= do_step,
};
