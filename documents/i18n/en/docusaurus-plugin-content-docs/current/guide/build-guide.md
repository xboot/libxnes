# Build and Install

This document describes how to build the libxnes library and example programs from source.

## Dependencies

The libxnes library itself only depends on the standard C library. Building the Linux example program requires SDL2:

```shell
sudo apt-get install libsdl2-dev
```

## Build

Run the following in the source root directory:

```shell
cd libxnes
make
```

After the build completes, the following will be generated:

- `examples/linux/output/xnes` — Linux SDL2 example program
- `examples/windows/output/xnes.exe` — Windows example program (cross-compiled)
- `examples/debugger/output/xnes-debugger` — Debugger example

## Build Only the Linux Example

```shell
make -C examples/linux
```

## Run

```shell
cd examples/linux/output/
./xnes <rom.nes>
```

You can also drag and drop a `.nes` file directly onto the program window to load it.

## Cross-Compiling the Windows Version

The Windows example is cross-compiled using MinGW, and the repository already includes the SDL2 Windows development libraries:

```shell
make -C examples/windows
```

## Integrating Into a Project

libxnes uses the `xnes.mk` Makefile snippet to make integration into other projects easy. Include it in your Makefile:

```makefile
sinclude <path-to-libxnes>/src/xnes.mk
```

This will automatically add the libxnes header path and source directory. Then include the header in your code:

```c
#include <xnes.h>
```

## Clean

```shell
make clean
```
