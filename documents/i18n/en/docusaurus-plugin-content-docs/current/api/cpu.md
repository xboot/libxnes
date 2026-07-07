# CPU Functions

libxnes implements a complete Ricoh 6502 (NMOS 6502) CPU instruction set emulation, including all legal instructions, illegal opcodes, and addressing modes.

## Data Structure

```c
struct xnes_cpu_t {
    struct xnes_ctx_t * ctx;
    uint8_t ram[2048];     // 2KB internal RAM
    uint64_t cycles;       // Cumulative cycle count
    uint32_t stall;        // DMA stall cycles
    uint16_t pc;           // Program counter
    uint8_t sp;            // Stack pointer
    uint8_t a;             // Accumulator
    uint8_t x;             // X index register
    uint8_t y;             // Y index register
    uint8_t p;             // Processor status flags
    uint8_t interrupt;     // Pending interrupt
    int (*debugger)(struct xnes_ctx_t *); // Debugger callback
};
```

## Status Flags

```c
enum {
    XNES_CPU_P_C = (1 << 0), // Carry
    XNES_CPU_P_Z = (1 << 1), // Zero
    XNES_CPU_P_I = (1 << 2), // Interrupt disable
    XNES_CPU_P_D = (1 << 3), // Decimal mode
    XNES_CPU_P_B = (1 << 4), // Break - BRK instruction
    XNES_CPU_P_U = (1 << 5), // Unused (always 1)
    XNES_CPU_P_V = (1 << 6), // Overflow
    XNES_CPU_P_N = (1 << 7), // Negative
};
```

## Functions

### xnes_cpu_init

```c
void xnes_cpu_init(struct xnes_cpu_t * cpu, struct xnes_ctx_t * ctx);
```

Initializes the CPU, zeroes the internal RAM, and sets initial register values. Usually called internally by `xnes_ctx_alloc`.

### xnes_cpu_reset

```c
void xnes_cpu_reset(struct xnes_cpu_t * cpu);
```

Resets the CPU. Reads the reset vector from addresses $FFFC and $FFFD to set the PC. Zeroes RAM, sets the stack pointer SP=$FD, and the status register P=$24 (I and U flags set).

### xnes_cpu_read8

```c
uint8_t xnes_cpu_read8(struct xnes_cpu_t * cpu, uint16_t addr);
```

Reads one byte from the CPU address space. Address decoding:
- $0000-$1FFF: Internal RAM (mirrored)
- $2000-$3FFF: PPU registers
- $4000-$4017: APU and controller registers
- $4020-$FFFF: Cartridge space

### xnes_cpu_write8

```c
void xnes_cpu_write8(struct xnes_cpu_t * cpu, uint16_t addr, uint8_t val);
```

Writes one byte to the CPU address space. Address decoding is the same as above.

### xnes_cpu_trigger_nmi

```c
void xnes_cpu_trigger_nmi(struct xnes_cpu_t * cpu);
```

Triggers an NMI (Non-Maskable Interrupt). The CPU responds before the next instruction, reading the NMI vector from $FFFA/$FFFB.

### xnes_cpu_trigger_irq

```c
void xnes_cpu_trigger_irq(struct xnes_cpu_t * cpu);
```

Triggers an IRQ (maskable interrupt). If the I flag is not set, the CPU responds before the next instruction, reading the IRQ vector from $FFFE/$FFFF.

### xnes_cpu_step

```c
int xnes_cpu_step(struct xnes_cpu_t * cpu);
```

Executes one CPU instruction. The **return value** is the number of CPU cycles consumed by this instruction (typically 2-7 cycles). The debugger callback is checked and invoked before execution.

Usually called internally by `xnes_step_frame`; no need to use directly.
