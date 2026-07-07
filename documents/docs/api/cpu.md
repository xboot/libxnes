# CPU 函数

libxnes 实现了完整的 Ricoh 6502（NMOS 6502）CPU 指令集模拟，包括所有合法指令、非法指令和寻址模式。

## 数据结构

```c
struct xnes_cpu_t {
    struct xnes_ctx_t * ctx;
    uint8_t ram[2048];     // 2KB 内部 RAM
    uint64_t cycles;       // 累计周期数
    uint32_t stall;        // DMA 停顿周期
    uint16_t pc;           // 程序计数器
    uint8_t sp;            // 栈指针
    uint8_t a;             // 累加器
    uint8_t x;             // X 索引寄存器
    uint8_t y;             // Y 索引寄存器
    uint8_t p;             // 处理器状态标志
    uint8_t interrupt;     // 待处理中断
    int (*debugger)(struct xnes_ctx_t *); // 调试器回调
};
```

## 状态标志

```c
enum {
    XNES_CPU_P_C = (1 << 0), // Carry - 进位
    XNES_CPU_P_Z = (1 << 1), // Zero - 零标志
    XNES_CPU_P_I = (1 << 2), // Interrupt - 中断禁止
    XNES_CPU_P_D = (1 << 3), // Decimal - 十进制模式
    XNES_CPU_P_B = (1 << 4), // Break - BRK 指令
    XNES_CPU_P_U = (1 << 5), // Unused - 未使用（始终为 1）
    XNES_CPU_P_V = (1 << 6), // Overflow - 溢出
    XNES_CPU_P_N = (1 << 7), // Negative - 负数
};
```

## 函数

### xnes_cpu_init

```c
void xnes_cpu_init(struct xnes_cpu_t * cpu, struct xnes_ctx_t * ctx);
```

初始化 CPU，清零内部 RAM，设置寄存器初始值。通常由 `xnes_ctx_alloc` 内部调用。

### xnes_cpu_reset

```c
void xnes_cpu_reset(struct xnes_cpu_t * cpu);
```

复位 CPU。从地址 $FFFC 和 $FFFD 读取复位向量，设置 PC。清零 RAM，设置栈指针 SP=$FD，状态寄存器 P=$24（I 和 U 标志置位）。

### xnes_cpu_read8

```c
uint8_t xnes_cpu_read8(struct xnes_cpu_t * cpu, uint16_t addr);
```

从 CPU 地址空间读取一个字节。地址译码：
- $0000-$1FFF：内部 RAM（镜像）
- $2000-$3FFF：PPU 寄存器
- $4000-$4017：APU 和控制器寄存器
- $4020-$FFFF：卡带空间

### xnes_cpu_write8

```c
void xnes_cpu_write8(struct xnes_cpu_t * cpu, uint16_t addr, uint8_t val);
```

向 CPU 地址空间写入一个字节。地址译码同上。

### xnes_cpu_trigger_nmi

```c
void xnes_cpu_trigger_nmi(struct xnes_cpu_t * cpu);
```

触发 NMI（不可屏蔽中断）。CPU 在下一条指令前响应，从 $FFFA/$FFFB 读取 NMI 向量。

### xnes_cpu_trigger_irq

```c
void xnes_cpu_trigger_irq(struct xnes_cpu_t * cpu);
```

触发 IRQ（可屏蔽中断）。如果 I 标志未置位，CPU 在下一条指令前响应，从 $FFFE/$FFFF 读取 IRQ 向量。

### xnes_cpu_step

```c
int xnes_cpu_step(struct xnes_cpu_t * cpu);
```

执行一条 CPU 指令。**返回值**为该指令消耗的 CPU 周期数（通常 2-7 个周期）。在执行前会检查并调用调试器回调。

通常由 `xnes_step_frame` 内部调用，无需直接使用。
