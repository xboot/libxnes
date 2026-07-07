# Supported Mappers

libxnes has 15 built-in NES mappers, covering a large number of common games.

## Mapper List

| Mapper Number | Name | Description | Typical Games |
|-----------|------|------|---------|
| 0 | NROM | The most basic mapper, no bank switching | Super Mario Bros. |
| 1 | MMC1 (SxROM) | PRG/CHR bank switching, programmable mirroring | The Legend of Zelda, Mega Man 2 |
| 2 | UxROM | PRG bank switching, single CHR | Mega Man, Castlevania |
| 3 | CNROM | CHR bank switching, single PRG | Duck Hunt, Arkanoid |
| 4 | MMC3 (TxROM) | Advanced bank switching, IRQ counter | Super Mario Bros. 3, Mega Man 3 |
| 7 | AxROM | Single PRG bank switching, four-screen mirroring | Battletoads, Marble Madness |
| 15 | 100-in-1 | Multi-game multicart | 100-in-1 multicart |
| 66 | GxROM | PRG + CHR bank switching | Doraemon |
| 79 | NINA-03/06 | NES 2.0 | American Video Entertainment games |
| 87 | Jaleco | CHR bank switching | Moero!! Pro Tennis |
| 113 | HES | PRG + CHR bank switching | HES games |
| 140 | Jaleco (early) | CHR bank switching | Bio Senshi Dan |
| 177 | Henggedianzi | PRG bank switching | Chinese NES games |
| 225 | 72-in-1 | Multi-game multicart | 72-in-1 multicart |
| 241 | Waixing | PRG bank switching | Waixing games |

## ROM Format

libxnes supports ROM files (`.nes`) in the NES 2.0 format. The ROM header contains the following information:

- Magic number: `NES\x1A`
- PRG-ROM size (in 16KB units)
- CHR-ROM size (in 8KB units)
- Mapper number (low 4 bits in flags6, high 4 bits in flags7)
- Mirroring mode (horizontal/vertical/four-screen)
- Timing type (NTSC/PAL/Dendy)
- PRG-RAM / NVRAM size
- CHR-RAM / NVRAM size

:::note
The NES 2.0 format is a superset of the iNES format (NES 1.0). libxnes is compatible with both formats.
:::
