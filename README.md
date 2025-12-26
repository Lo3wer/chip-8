## C Chip-8 emulator

Followed specification from Tobias Langhoff's guide (https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) to create fully functional emulator for Chip-8 games. 
Right now it supports basic functionalities and has configurable values/behaviours for ambiguous instructions.
Includes comprehensive unit tests for the cpu which includes every possible instruction plus timer behaviours.

## Build & Run (Windows)

Requirements:
- CMake ≥ 3.16
- A C compiler toolchain (e.g., MSVC via “x64 Native Tools Command Prompt”, or MinGW/Clang)
- No external SDL install needed (SDL is vendored under `vendored/SDL`)

### Configure & build
```bash
cmake -S . -B build -G "Ninja"   # or "MinGW Makefiles" / "Visual Studio 17 2022"
cmake --build build --config Release
```

### Run emulator
```bash
# From the build output dir (adjust config if not Release)
./build/Release/chip8.exe path\to\rom.ch8
```

### Run tests
```bash
cd build
ctest --config Release
```
