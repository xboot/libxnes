# 支持的映射器

libxnes 内置 15 种 NES 映器，覆盖了大量常见游戏。

## 映射器列表

| 映射器编号 | 名称 | 说明 | 典型游戏 |
|-----------|------|------|---------|
| 0 | NROM | 最基础的映射器，无 bank 切换 | Super Mario Bros. |
| 1 | MMC1 (SxROM) | 支持 PRG/CHR bank 切换、可编程镜像 | The Legend of Zelda, Mega Man 2 |
| 2 | UxROM | PRG bank 切换，单 CHR | Mega Man, Castlevania |
| 3 | CNROM | CHR bank 切换，单 PRG | Duck Hunt, Arkanoid |
| 4 | MMC3 (TxROM) | 高级 bank 切换、IRQ 计数器 | Super Mario Bros. 3, Mega Man 3 |
| 7 | AxROM | 单 PRG bank 切换，四屏镜像 | Battletoads, Marble Madness |
| 15 | 100-in-1 | 多游戏合卡 | 100-in-1 合卡 |
| 66 | GxROM | PRG + CHR bank 切换 | Doraemon |
| 79 | NINA-03/06 | NES 2.0 | American Video Entertainment 游戏 |
| 87 | Jaleco | CHR bank 切换 | Moero!! Pro Tennis |
| 113 | HES | PRG + CHR bank 切换 | HES 游戏 |
| 140 | Jaleco (early) | CHR bank 切换 | Bio Senshi Dan |
| 177 | Henggedianzi | PRG bank 切换 | 中文 NES 游戏 |
| 225 | 72-in-1 | 多游戏合卡 | 72-in-1 合卡 |
| 241 | Waixing | PRG bank 切换 | Waixing 游戏 |

## ROM 格式

libxnes 支持 NES 2.0 格式的 ROM 文件（`.nes`）。ROM 头部包含以下信息：

- 魔数：`NES\x1A`
- PRG-ROM 大小（16KB 单位）
- CHR-ROM 大小（8KB 单位）
- 映射器编号（低 4 位在 flags6，高 4 位在 flags7）
- 镜像模式（水平/垂直/四屏）
- 时序类型（NTSC/PAL/Dendy）
- PRG-RAM / NVRAM 大小
- CHR-RAM / NVRAM 大小

:::note
NES 2.0 格式是 iNES 格式（NES 1.0）的超集，libxnes 同时兼容两种格式。
:::
