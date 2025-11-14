# CHIP‑8 Emulator

[![C++](https://img.shields.io/badge/language-C%2B%2B-blue)](https://isocpp.org/)
[![SDL3](https://img.shields.io/badge/SDL-3.0-brightgreen)](https://www.libsdl.org/)


A fully functional CHIP‑8 emulator written in C++ that passes the [CHIP‑8 Test Suite](https://github.com/Timendus/chip8-test-suite) and runs almost all CHIP‑8 games. Designed for learning, fun, experimentation, and demonstrating system-level programming skills.

---

![Demo of the emulator](documentation/emulator_example.gif)

---

## ✨ Features
- Full CHIP‑8 instruction set implemented
- Passes all tests from the [CHIP‑8 Test Suite](https://github.com/Timendus/chip8-test-suite)
- Runs almost all CHIP‑8 ROMs (games & programs)
- Cross-platform (Windows, Linux, macOS)
- Minimal external dependencies

---

## 🛠 Getting Started

### Prerequisites
- C++11 compatible compiler (e.g. g++, clang++, MSVC)
- CMake 3.x or later
- SDL3 for graphics, audio(beep) and input

### Building
```bash
git clone https://github.com/VedadSiljic/chip8-emulator.git
cd chip8-emulator
mkdir build && cd build
cmake ..
make
```

### Running
```bash
./chip-8 -d "<PATH_TO_ROM_FILE>"
```
`-d` : (optional) Enables debug mode (prints keyboard state in the terminal).
Replace the <PATH_TO_ROM_FILE> with the location of the rom file.

## 🙏 Acknowledgements

-   [CHIP‑8 Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)

-   [CHIP‑8 Test Suite](https://github.com/Timendus/chip8-test-suite)
