# Space Invaders Engine

SDL2/C++17 framework scaffold for a Space Invaders game. It contains engine services only;
gameplay entities and scenes are intentionally left for future development.

## Build

Install SDL2, SDL2_image, SDL2_ttf, and SDL2_mixer so CMake can discover their config packages,
then run:

```powershell
cmake -S . -B build
cmake --build build
```
