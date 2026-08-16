# Space Invaders Engine

SDL3/C++17 framework scaffold for a Space Invaders game. It contains engine services only;
gameplay entities and scenes are intentionally left for future development.

## Change Log

### 2024-05-21

- Finalized 5-wave gameplay, including Wave 5 "Swarm Assault" dive pattern and specific entry/movement patterns for all waves.
- Modified files: `src/Scenes/GameSceneCombat.cpp`, `src/Scenes/GameSceneWave.cpp`.

### 2024-05-21

- Extended wave system to support a 5-wave gameplay configuration.
- Modified files: `src/Scenes/GameScene.cpp`, `src/Scenes/GameSceneWave.cpp`.

### 2024-05-21

- Increased the number of enemies in each wave to provide scaling difficulty.
- Modified files: `src/Scenes/GameSceneWave.cpp`.

### 2024-05-21

- Implemented specific enemy count and type composition for 5 waves.
- Modified files: `src/Scenes/GameScene.cpp`, `src/Scenes/GameSceneWave.cpp`.

### 2024-05-21

- Upgraded enemy gameplay mechanics:
- Implemented scaling wave difficulty.
- Added new swarm movement patterns (ZigZag, Vortex, Expansion/Contraction).
- Implemented Kamikaze/Dive-Bomb attacks for enemies.
- Added a delayed wave transition with a fly-by preview effect.
- Modified files: `include/Entities/Enemy.h`, `src/Entities/Enemy.cpp`, `include/Scenes/GameScene.h`, `src/Scenes/GameScene.cpp`, `src/Scenes/GameSceneWave.cpp`, `src/Scenes/GameSceneCombat.cpp`, `src/Scenes/GameSceneUI.cpp`.

### 2024-XX-XX

- Upgraded enemy entrance trajectories to use smooth Bezier curves for a "Galaga-style" feel.
- Modified files: `include/Entities/Enemy.h`, `src/Entities/Enemy.cpp`, `src/Scenes/GameSceneWave.cpp`.

## Build

Install SDL3, SDL3_image, SDL3_ttf, and SDL3_mixer so CMake can discover their config packages,
then run:

```powershell
cmake -B build -G "MinGW Makefiles"
cmake -S . -B build
cmake --build build
```
