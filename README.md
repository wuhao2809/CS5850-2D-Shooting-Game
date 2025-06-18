# 2D Shooting Game

A modern 2D shooting game built with C++ and SDL3, featuring an Entity Component System (ECS) architecture. Hunt ducks for points while avoiding collisions in this fast-paced 60-second challenge!

## 🎮 Game Features

- **Duck Shooting Gameplay**: Shoot ducks while avoiding collisions
- **60-Second Time Challenge**: Race against the clock to achieve the highest score
- **Multiple Target Types**: Regular ducks (10 points) and boss ducks (50 points)
- **Smooth 8-Directional Movement**: Full directional control with sprite rotation
- **Precise Collision Detection**: AABB collision system with adjustable hitboxes
- **Score Tracking**: Real-time score, accuracy percentage, and persistent high scores
- **Dynamic Spawning**: Targets spawn from all screen edges with varied patterns

## 🕹️ Controls

| Action       | Keys                    |
| ------------ | ----------------------- |
| Move         | Arrow Keys or WASD      |
| Shoot        | Space Bar               |
| Toggle HUD   | H                       |
| Toggle Debug | ESC (cycles log levels) |
| Quit         | Q (when game is over)   |

## 🎯 How to Play

1. **Objective**: Score as many points as possible within 60 seconds
2. **Movement**: Use arrow keys or WASD to move in 8 directions
3. **Shooting**: Press space to shoot projectiles in your facing direction
4. **Scoring**: Hit ducks to earn points (Regular: 10pts, Boss: 50pts)
5. **Game Over**: Avoid touching any ducks! Game ends on collision or timeout
6. **Win Strategy**: Balance aggressive shooting with careful positioning

## 🛠️ Technical Architecture

### Entity Component System (ECS)

The game uses a pure ECS architecture for maximum flexibility and performance:

```
Entity: Unique ID representing a game object
Component: Data-only structures (position, sprite, health, etc.)
System: Logic that operates on entities with specific components
```

### Core Systems Overview

| System                  | Responsibility                             | Key Components Used                   |
| ----------------------- | ------------------------------------------ | ------------------------------------- |
| **PlayerControlSystem** | Handles input and player movement          | Player, Input, Transform, Movement    |
| **TargetSpawnSystem**   | Spawns ducks with configured patterns      | Transform, Target, Movement           |
| **DuckMovementSystem**  | Controls duck AI movement toward player    | Transform, Movement, Target           |
| **CollisionSystem**     | Detects collisions between entities        | Transform, Collision, Sprite          |
| **GameStateSystem**     | Manages game state and win/lose conditions | CollisionResult, ShootingGalleryState |
| **ProjectileSystem**    | Handles projectile lifecycle               | Transform, Movement, Projectile       |
| **RenderSystem**        | Renders all visual entities                | Transform, Sprite, Images             |

## 🚀 Building and Running

### Prerequisites

- **C++17** or later
- **CMake 3.10** or later
- **SDL3** development libraries
- **SDL3_ttf** development libraries
- **nlohmann/json** (fetched automatically by CMake)

### Quick Start

```bash
# Clone the repository
git clone https://github.com/wuhao2809/CS5850-2D-Shooting-Game.git
cd CS5850-2D-Shooting-Game

# Run the build script
./run.sh

# Or build manually
mkdir build && cd build
cmake ..
make -j4
./bin/GameEngine
```

### Platform-Specific Installation

<details>
<summary><b>macOS (Homebrew)</b></summary>

```bash
brew install sdl3 sdl3_ttf cmake
```

</details>

<details>
<summary><b>Ubuntu/Debian</b></summary>

```bash
sudo apt-get update
sudo apt-get install libsdl3-dev libsdl3-ttf-dev cmake build-essential
```

</details>

<details>
<summary><b>Windows</b></summary>

1. Install Visual Studio 2019 or later
2. Install CMake from [cmake.org](https://cmake.org/download/)
3. Download SDL3 development libraries
4. Set SDL3_DIR and SDL3_TTF_DIR environment variables
</details>

## 🎨 Game Configuration

Edit `GameAssets/GameData.json` to customize:

```json
{
  "world": {
    "width": 800,
    "height": 600
  },
  "player": {
    "moveSpeed": 300,
    "fireRate": 0.5
  },
  "templates": {
    "duck_regular": {
      "pointValue": 10,
      "speed": 100
    },
    "duck_boss": {
      "pointValue": 50,
      "speed": 50
    }
  }
}
```

## 🐛 Troubleshooting

| Issue                | Solution                                                          |
| -------------------- | ----------------------------------------------------------------- |
| **Build fails**      | Ensure all dependencies are installed and CMake version is 3.10+  |
| **Game won't start** | Check if GameAssets folder is in the same directory as executable |
| **Missing SDL3**     | Verify SDL3 is installed and in your system's library path        |
| **Black screen**     | Check console for asset loading errors                            |

## 📝 Development Notes

- **Adding Components**: Create in `src/game/ecs/components/`, inherit from `Component`
- **Adding Systems**: Create in `src/game/ecs/systems/`, inherit from `System`
- **Registering Systems**: Add to `GameWorld::initialize()` in proper order
- **Debug Mode**: Press ESC to cycle through log levels for debugging

## 📄 License

This project is developed for educational purposes as part of CS5850 coursework.

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 👤 Author

- **Hao Wu** - [wuhao2809](https://github.com/wuhao2809)
