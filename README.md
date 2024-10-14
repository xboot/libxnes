

***
# Libxnes
A lightweight, portable pure `C99` NES emulator library.

## Getting Started

Just type `make` at the root directory, you will see some binary of [examples](examples) for usage.

```shell
cd libxnes
make
```

To compile the example, you need to install SDL2, In ubuntu systems:

```shell
sudo apt-get install libsdl2-dev
```

#### How to running

After compiling all of files, you can running an example or drag some nes file to xnes:

```shell
cd examples/linux/output/
./xnes <rom.nes>
```

## Screenshots
![super-mario](documents/screenshots/super-mario.png)
![contra](documents/screenshots/contra.png)
![battle-city](documents/screenshots/battle-city.png)
![flying-hero](documents/screenshots/flying-hero.png)
![gun-smoke](documents/screenshots/gun-smoke.png)
![jackal](documents/screenshots/jackal.png)
![1943](documents/screenshots/1943.png)
![circus-chablic](documents/screenshots/circus-chablic.png)
![chinese-chese](documents/screenshots/chinese-chese.png)
![lode-runner](documents/screenshots/lode-runner.png)
![pinball](documents/screenshots/pinball.png)
![excite-bike](documents/screenshots/excite-bike.png)
![bomber-man](documents/screenshots/bomber-man.png)
![3eye](documents/screenshots/3eye.png)
![chip-dale](documents/screenshots/chip-dale.png)
![duck-hunt](documents/screenshots/duck-hunt.png)
![64-in-1](documents/screenshots/64-in-1.png)

## Keyboard

| Description | Player 1 | Player 2 |
| ---- | ---- | :--- |
| UP   | W         | Arrow Up |
| DOWN | S       | Arrow Down |
| LEFT | A       | Arrow Left |
| RIGHT | D     | Arrow Right |
| A    | K        | Numpad 6 |
| B    | J        | Numpad 5    |
| SELECT | Space | Numpad 8 |
| START | Enter  | Numpad 9 |

| Description | Key |
| :--- | ---- |
| RESET | ESC    |
| SLOW SPEED | F1       |
| FAST SPEED | F2       |
| REWIND | R       |

## Joystick

| Description | Joystick 1 | Joystick 2 |
| ---- | ---- | :--- |
| UP   | Up     | Up |
| DOWN | Down   | Down |
| LEFT | Left | Left |
| RIGHT | Right | Right |
| A    | A         | A |
| B    | B        | B   |
| SELECT | Select | Select |
| START | Start | Start |
| REWIND | L      |L|
| FAST SPEED | R      |R|

## Links

* [The Chinese Discussion Posts](https://whycan.com/t_11467.html)
* [The Nesdev Wiki](https://www.nesdev.org/wiki/Nesdev_Wiki)
* [The 6502 Instruction Set](https://www.masswerk.at/6502/6502_instruction_set.html)
* [The NES Cart Database](https://nescartdb.com)

## License

This library is free software; you can redistribute it and or modify it under the terms of the MIT license. See [MIT License](LICENSE) for details.

