# Cartridge Functions

The cartridge system is responsible for loading NES ROM data and implementing address decoding and bank switching through mappers. libxnes supports NES 2.0 format ROMs and includes 15 built-in mappers.

## Data Structure

```c
struct xnes_cartridge_t {
    struct { /* NES 2.0 header */ } header;
    uint16_t mapper_number;    // Mapper number
    uint8_t mirror;            // Mirror mode
    uint8_t timing;            // Timing type (NTSC/PAL/Dendy)
    uint8_t * prg_rom;         // PRG-ROM data
    uint8_t * prg_ram;         // PRG-RAM (battery-backed)
    uint8_t * chr_rom;         // CHR-ROM / CHR-RAM data
    // ...
    struct { /* Mapper state and callbacks */ } mapper;
};
```

## Mirror Modes

```c
enum {
    XNES_CARTRIDGE_MIRROR_HORIZONTAL  = 0, // Horizontal mirroring
    XNES_CARTRIDGE_MIRROR_VERTICAL    = 1, // Vertical mirroring
    XNES_CARTRIDGE_MIRROR_FOUR_SCREEN = 2, // Four-screen
    XNES_CARTRIDGE_MIRROR_SINGLE0     = 3, // Single screen (nametable 0)
    XNES_CARTRIDGE_MIRROR_SINGLE1     = 4, // Single screen (nametable 1)
};
```

## Timing Types

```c
enum {
    XNES_CARTRIDGE_TIMING_NTSC   = 0, // NTSC (60 Hz, 29780 cycles/frame)
    XNES_CARTRIDGE_TIMING_PAL    = 1, // PAL (50 Hz, 33260 cycles/frame)
    XNES_CARTRIDGE_TIMING_DENDY  = 2, // Dendy (50 Hz, 35464 cycles/frame)
};
```

## Functions

### xnes_cartridge_alloc

```c
struct xnes_cartridge_t * xnes_cartridge_alloc(const void * buf, size_t len, struct xnes_ctx_t * ctx);
```

Creates a cartridge from ROM data. Parses the NES 2.0 header, allocates memory for PRG-ROM, CHR-ROM, PRG-RAM, etc., and initializes the corresponding mapper based on the mapper number. Returns `NULL` if parsing fails.

Usually called internally by `xnes_ctx_alloc`; no need to use directly.

### xnes_cartridge_free

```c
void xnes_cartridge_free(struct xnes_cartridge_t * c);
```

Frees the cartridge and all its allocated memory.

### Mapper Interface

The mapper provides read/write callbacks for the CPU/PPU address space through function pointers:

```c
uint8_t xnes_cartridge_mapper_cpu_read(struct xnes_ctx_t * ctx, uint16_t addr);
void xnes_cartridge_mapper_cpu_write(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val);
uint8_t xnes_cartridge_mapper_ppu_read(struct xnes_ctx_t * ctx, uint16_t addr);
void xnes_cartridge_mapper_ppu_write(struct xnes_ctx_t * ctx, uint16_t addr, uint8_t val);
void xnes_cartridge_mapper_apu_step(struct xnes_ctx_t * ctx);
void xnes_cartridge_mapper_ppu_step(struct xnes_ctx_t * ctx);
```

These functions are called by the CPU, PPU, and APU in the emulation loop; the mapper performs bank switching and address decoding according to its own logic. Some mappers (such as Mapper 4) check IRQ conditions during APU or PPU steps.

## Supported Mappers

For the complete list, see the [Mapper Reference List](../reference/mappers).
