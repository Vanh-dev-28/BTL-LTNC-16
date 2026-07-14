# Space Invaders Engine

SDL3/C++17 framework scaffold for a Space Invaders game. It contains engine services only;
gameplay entities and scenes are intentionally left for future development.

## Build

Install SDL3, SDL3_image, SDL3_ttf, and SDL3_mixer so CMake can discover their config packages,
then run:

```powershell
cmake -S . -B build
cmake --build build
```
