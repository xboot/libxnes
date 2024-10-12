#include <debugger.h>
#include <context.h>
#include <command.h>

enum {
	CPU_ADDR_MODE_NONE				= 0,
	CPU_ADDR_MODE_ABSOLUTE			= 1,
	CPU_ADDR_MODE_ABSOLUTE_X		= 2,
	CPU_ADDR_MODE_ABSOLUTE_Y		= 3,
	CPU_ADDR_MODE_ACCUMULATOR		= 4,
	CPU_ADDR_MODE_IMMEDIATE			= 5,
	CPU_ADDR_MODE_IMPLIED			= 6,
	CPU_ADDR_MODE_INDIRECT_X		= 7,
	CPU_ADDR_MODE_INDIRECT			= 8,
	CPU_ADDR_MODE_INDIRECT_Y		= 9,
	CPU_ADDR_MODE_RELATIVE			= 10,
	CPU_ADDR_MODE_ZEROPAGE			= 11,
	CPU_ADDR_MODE_ZEROPAGE_X		= 12,
	CPU_ADDR_MODE_ZEROPAGE_Y		= 13,
};

static const uint8_t cpu_instruction_mode[256] = {
	      /* 00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F */
	/* 00 */  6,  7,  6,  7, 11, 11, 11, 11,  6,  5,  4,  5,  1,  1,  1,  1,
	/* 10 */ 10,  9,  6,  9, 12, 12, 12, 12,  6,  3,  6,  3,  2,  2,  2,  2,
	/* 20 */  1,  7,  6,  7, 11, 11, 11, 11,  6,  5,  4,  5,  1,  1,  1,  1,
	/* 30 */ 10,  9,  6,  9, 12, 12, 12, 12,  6,  3,  6,  3,  2,  2,  2,  2,
	/* 40 */  6,  7,  6,  7, 11, 11, 11, 11,  6,  5,  4,  5,  1,  1,  1,  1,
	/* 50 */ 10,  9,  6,  9, 12, 12, 12, 12,  6,  3,  6,  3,  2,  2,  2,  2,
	/* 60 */  6,  7,  6,  7, 11, 11, 11, 11,  6,  5,  4,  5,  8,  1,  1,  1,
	/* 70 */ 10,  9,  6,  9, 12, 12, 12, 12,  6,  3,  6,  3,  2,  2,  2,  2,
	/* 80 */  5,  7,  5,  7, 11, 11, 11, 11,  6,  5,  6,  5,  1,  1,  1,  1,
	/* 90 */ 10,  9,  6,  9, 12, 12, 13, 13,  6,  3,  6,  3,  2,  2,  3,  3,
	/* A0 */  5,  7,  5,  7, 11, 11, 11, 11,  6,  5,  6,  5,  1,  1,  1,  1,
	/* B0 */ 10,  9,  6,  9, 12, 12, 13, 13,  6,  3,  6,  3,  2,  2,  3,  3,
	/* C0 */  5,  7,  5,  7, 11, 11, 11, 11,  6,  5,  6,  5,  1,  1,  1,  1,
	/* D0 */ 10,  9,  6,  9, 12, 12, 12, 12,  6,  3,  6,  3,  2,  2,  2,  2,
	/* E0 */  5,  7,  5,  7, 11, 11, 11, 11,  6,  5,  6,  5,  1,  1,  1,  1,
	/* F0 */ 10,  9,  6,  9, 12, 12, 12, 12,  6,  3,  6,  3,  2,  2,  2,  2,
};

static const uint8_t cpu_instruction_size[256] = {
	      /* 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F */
	/* 00 */ 1, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* 10 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* 20 */ 3, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* 30 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* 40 */ 1, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* 50 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* 60 */ 1, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* 70 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* 80 */ 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* 90 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* A0 */ 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* B0 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* C0 */ 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* D0 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
	/* E0 */ 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
	/* F0 */ 2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
};

static const uint8_t cpu_instruction_name[256][4] = {
	       /* 00    01      02     03     04     05     06     07     08     09     0A     0B     0C     0D     0E     0F */
	/* 00 */ "BRK", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO", "PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO",
	/* 10 */ "BPL", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO", "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
	/* 20 */ "JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA", "PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA",
	/* 30 */ "BMI", "AND", "KIL", "RLA", "NOP", "AND", "ROL", "RLA", "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",
	/* 40 */ "RTI", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE", "PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE",
	/* 50 */ "BVC", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE", "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
	/* 60 */ "RTS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA", "PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
	/* 70 */ "BVS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA", "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",
	/* 80 */ "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX", "DEY", "NOP", "TXA", "XAA", "STY", "STA", "STX", "SAX",
	/* 90 */ "BCC", "STA", "KIL", "AHX", "STY", "STA", "STX", "SAX", "TYA", "STA", "TXS", "TAS", "SHY", "STA", "SHX", "AHX",
	/* A0 */ "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX", "TAY", "LDA", "TAX", "LAX", "LDY", "LDA", "LDX", "LAX",
	/* B0 */ "BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX", "CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX",
	/* C0 */ "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP", "INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
	/* D0 */ "BNE", "CMP", "KIL", "DCP", "NOP", "CMP", "DEC", "DCP", "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
	/* E0 */ "CPX", "SBC", "NOP", "ISC", "CPX", "SBC", "INC", "ISC", "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISC",
	/* F0 */ "BEQ", "SBC", "KIL", "ISC", "NOP", "SBC", "INC", "ISC", "SED", "SBC", "NOP", "ISC", "NOP", "SBC", "INC", "ISC",
};

static inline uint16_t cpu_read16(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t lo = xnes_cpu_read8(cpu, addr);
	uint8_t hi = xnes_cpu_read8(cpu, addr + 1);
	return ((uint16_t)hi << 8) | lo;
}

static inline uint16_t cpu_read16_indirect_bug(struct xnes_cpu_t * cpu, uint16_t addr)
{
	uint8_t lo = xnes_cpu_read8(cpu, addr);
	uint8_t hi = xnes_cpu_read8(cpu, (addr & 0xff00) | ((addr + 1) & 0x00ff));
	return ((uint16_t)hi << 8) | lo;
}

static int dasm(struct xnes_ctx_t * ctx, uint16_t pc)
{
	struct xnes_cpu_t * cpu = &ctx->cpu;
	uint8_t opcode = xnes_cpu_read8(cpu, pc);
	uint8_t mode = cpu_instruction_mode[opcode];
	uint8_t bytes = cpu_instruction_size[opcode];
	uint16_t addr;
	uint8_t val;

	if(bytes == 0)
		shell_printf("%04X  %02X      ", pc, xnes_cpu_read8(cpu, pc + 0));
	else if(bytes == 1)
		shell_printf("%04X  %02X      ", pc, xnes_cpu_read8(cpu, pc + 0));
	else if(bytes == 2)
		shell_printf("%04X  %02X %02X   ", pc, xnes_cpu_read8(cpu, pc + 0), xnes_cpu_read8(cpu, pc + 1));
	else if(bytes == 3)
		shell_printf("%04X  %02X %02X %02X", pc, xnes_cpu_read8(cpu, pc + 0), xnes_cpu_read8(cpu, pc + 1), xnes_cpu_read8(cpu, pc + 2));
	shell_printf("  %s ", (char *)cpu_instruction_name[opcode]);

	switch(mode)
	{
	case CPU_ADDR_MODE_ABSOLUTE:
		addr = cpu_read16(cpu, pc + 1);
		shell_printf("$%04X", addr);
		break;
	case CPU_ADDR_MODE_ABSOLUTE_X:
		addr = cpu_read16(cpu, pc + 1);
		shell_printf("$%04X,X", addr);
		break;
	case CPU_ADDR_MODE_ABSOLUTE_Y:
		addr = cpu_read16(cpu, pc + 1);
		shell_printf("$%04X,Y", addr);
		break;
	case CPU_ADDR_MODE_ACCUMULATOR:
		shell_printf("A");
		break;
	case CPU_ADDR_MODE_IMMEDIATE:
		addr = pc + 1;
		val = xnes_cpu_read8(cpu, addr);
		shell_printf("#$%02X", val);
		break;
	case CPU_ADDR_MODE_IMPLIED:
		break;
	case CPU_ADDR_MODE_INDIRECT_X:
		addr = (uint16_t)xnes_cpu_read8(cpu, pc + 1);
		shell_printf("($%02X,X)", addr);
		break;
	case CPU_ADDR_MODE_INDIRECT:
		addr = cpu_read16_indirect_bug(cpu, cpu_read16(cpu, pc + 1));
		shell_printf("($%04X)", addr);
		break;
	case CPU_ADDR_MODE_INDIRECT_Y:
		addr = (uint16_t)xnes_cpu_read8(cpu, pc + 1);
		shell_printf("($%02X),Y", addr);
		break;
	case CPU_ADDR_MODE_RELATIVE:
		val = xnes_cpu_read8(cpu, pc + 1);
		if(val < 0x80)
			addr = pc + 2 + val;
		else
			addr = pc + 2 + val - 0x100;
		shell_printf("$%04X", addr);
		break;
	case CPU_ADDR_MODE_ZEROPAGE:
		addr = xnes_cpu_read8(cpu, pc + 1);
		shell_printf("$%02X", addr);
		break;
	case CPU_ADDR_MODE_ZEROPAGE_X:
		addr = xnes_cpu_read8(cpu, pc + 1);
		shell_printf("$%02X,X", addr);
		break;
	case CPU_ADDR_MODE_ZEROPAGE_Y:
		addr = xnes_cpu_read8(cpu, pc + 1);
		shell_printf("$%02X,Y", addr);
		break;
	default:
		break;
	}
	shell_printf("\r\n");

	return bytes;
}

static void usage(void)
{
	shell_printf("usage:\r\n");
	shell_printf("    dasm [count]\r\n");
}

static int do_dasm(struct xnes_ctx_t * ctx, int argc, char ** argv)
{
	int count = 1;
	int bytes = 0;

	if(argc > 1)
		count = strtoul(argv[1], NULL, 0);
	if(count < 1)
		count = 1;

	for(int i = 0; i < count; i++)
		bytes += dasm(ctx, ctx->cpu.pc + bytes);
	return 0;
}

struct command_t cmd_dasm = {
	.name	= "dasm",
	.desc	= "show the dasmrmation of nes",
	.usage	= usage,
	.exec	= do_dasm,
};
