#include <debugger.h>
#include <context.h>
#include <command.h>

static inline unsigned char tohex(unsigned char v)
{
	return ((v < 10) ? (v + '0') : (v - 10 + 'a'));
}

static void hexdump(struct xnes_ctx_t * ctx, uint16_t base, int len)
{
	uint16_t o = base;
	for(int n = 0; n < len; n += 16, o += 16)
	{
		shell_putchar(tohex((o >> 12) & 0xf));
		shell_putchar(tohex((o >>  8) & 0xf));
		shell_putchar(tohex((o >>  4) & 0xf));
		shell_putchar(tohex((o >>  0) & 0xf));
		shell_putchar(':');
		shell_putchar(' ');
		for(int i = 0; i < 16; i++)
		{
			if(n + i < len)
			{
				unsigned char c = xnes_cpu_read8(&ctx->cpu, base + n + i);
				shell_putchar(tohex((c >> 4) & 0xf));
				shell_putchar(tohex((c >> 0) & 0xf));
				shell_putchar(' ');
			}
			else
			{
				shell_putchar(' ');
				shell_putchar(' ');
				shell_putchar(' ');
			}
		}
		shell_putchar('|');
		for(int i = 0; i < 16; i++)
		{
			if(n + i >= len)
				shell_putchar(' ');
			else
			{
				unsigned char c = xnes_cpu_read8(&ctx->cpu, base + n + i);
				if(((unsigned int)c - 0x20) < 0x5f)
					shell_putchar(c);
				else
					shell_putchar('.');
			}
		}
		shell_putchar('\r');
		shell_putchar('\n');
	}
}

static void usage(void)
{
	shell_printf("usage:\r\n");
	shell_printf("    dump <address> <length>\r\n");
}

static int do_dump(struct xnes_ctx_t * ctx, int argc, char ** argv)
{
	uint16_t address = 0;
	int length = 16;

	if(argc > 1)
		address = strtoul(argv[1], NULL, 0);
	if(argc > 2)
		length = strtoul(argv[2], NULL, 0);
	hexdump(ctx, address, length);

	return 0;
}

struct command_t cmd_dump = {
	.name	= "dump",
	.desc	= "dump the memory of cpu bus",
	.usage	= usage,
	.exec	= do_dump,
};
