# State 函数

libxnes 提供状态保存/恢复功能，可用于实现倒带（Rewind）和即时存档。状态数据包含 CPU、DMA、PPU、APU 和卡带的完整快照。

## 数据结构

```c
struct xnes_state_t {
    struct xnes_ctx_t * ctx;  // 关联的模拟器上下文
    unsigned char * buffer;   // 状态环形缓冲区
    uint32_t length;          // 单个状态的大小
    uint32_t count;           // 缓冲区容量（状态数）
    uint32_t in;              // 写入位置
    uint32_t out;             // 读取位置
};
```

状态缓冲区是一个环形队列，`in` 和 `out` 为写入和读取索引。当队列满时，最旧的状态会被自动丢弃。

## 函数

### xnes_state_alloc

```c
struct xnes_state_t * xnes_state_alloc(struct xnes_ctx_t * ctx, int count);
```

创建状态管理器。`count` 指定缓冲区容量（可保存的状态数量）。例如 `count = 60 * 30` 表示保存最近 30 秒的状态（60 帧/秒）。

```c
/* 保存最近 30 秒的状态（用于倒带） */
struct xnes_state_t * state = xnes_state_alloc(nes, 60 * 30);
```

### xnes_state_free

```c
void xnes_state_free(struct xnes_state_t * state);
```

释放状态管理器及其缓冲区。传入 `NULL` 安全。

### xnes_state_push

```c
void xnes_state_push(struct xnes_state_t * state);
```

保存当前模拟器状态（压栈）。通常在每帧模拟完成后调用。如果缓冲区已满，最旧的状态会被丢弃。

```c
xnes_step_frame(nes);    // 模拟一帧
xnes_state_push(state);  // 保存状态
```

### xnes_state_pop

```c
void xnes_state_pop(struct xnes_state_t * state);
```

恢复上一个保存的状态（出栈）。恢复后，模拟器状态回到保存时的精确状态。

```c
xnes_state_pop(state);   // 回退一帧
```

## 倒带实现

通过持续 push 和 pop 实现倒带功能：

```c
/* 正向游戏循环 */
while(!rewind)
{
    uint64_t elapsed = xnes_step_frame(nes);
    xnes_state_push(state);
    /* 等待 elapsed 纳秒 */
}

/* 倒带循环 */
while(rewind)
{
    xnes_state_pop(state);
    /* 显示当前帧 */
    /* 控制倒带速度 */
}
```

:::note
恢复状态时会自动重置控制器，避免输入状态残留。
:::

## 状态内容

每个状态快照包含以下数据：

| 组件 | 内容 |
|------|------|
| CPU | 寄存器、RAM、PC、SP、周期数 |
| DMA | DMA 状态 |
| PPU | 寄存器、命名表、OAM、调色板、渲染状态 |
| APU | 所有声道状态、帧计数器 |
| Cartridge | 映射器状态、SRAM、PRG-RAM、CHR-RAM |

状态大小取决于卡带的内存配置（PRG-RAM、CHR-RAM 大小），通常在几 KB 到几十 KB 之间。
