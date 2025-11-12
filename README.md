# 3D Virtual Mountains - Terrain Generator

A procedural terrain generator implementing the **Diamond-Square algorithm** with isometric 3D visualization. Built with [Raylib](https://www.raylib.com/) for cross-platform graphics rendering.

**This project was developed as a learning exercise to explore and master the Raylib graphics library**, focusing on 2D/3D projections, real-time rendering, and interactive graphics programming.

![3d world](https://www.colpodigenio.it/github/terragen/terragenhalf.png)

## Features

- **Procedural Generation**: Uses the Diamond-Square algorithm for realistic terrain generation
- **Isometric Projection**: 3D terrain displayed with proper isometric perspective
- **Auto-Centering**: Automatically centers and scales the terrain to fit the screen
- **Height-Based Coloring**: Terrain colored by elevation (water, sand, grass, rock, snow)
- **Interactive**: Press SPACE to generate new terrain
- **Reference Axes**: Visual X, Y, Z axes for orientation

## Algorithm

The Diamond-Square algorithm is a classic fractal terrain generation method that works by:
1. Starting with a square grid with values at the corners
2. Performing "square" steps (averaging corners + random offset)
3. Performing "diamond" steps (averaging edges + random offset)
4. Recursively subdividing until the desired resolution is reached

## Requirements

- [Raylib](https://www.raylib.com/) library (version 4.0 or higher)
- C compiler (GCC, Clang, or MSVC)

## Compilation

### Linux/macOS

```bash
# Using the provided Makefile
make

# Or manually
gcc terragen.c -o terragen -lraylib -lm
```

### Windows (MinGW)

```bash
gcc terragen.c -o terragen.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```

### Windows (MSVC)

```bash
cl terragen.c /link raylib.lib opengl32.lib gdi32.lib winmm.lib
```

## Usage

Run the compiled executable:

```bash
./terragen
```

### Controls

- **SPACE**: Generate new terrain
- **ESC**: Exit application

## Configuration

You can modify the following constants in the source code:

- `ITERATIONS`: Grid resolution (must be 2^n+1, e.g., 65, 129, 257)
- `INITIAL_HEIGHT`: Starting amplitude for terrain generation
- `ROUGHNESS`: Controls terrain smoothness (lower = smoother)
- `ISO_ANGLE`: Isometric projection angle
- `ROTATION_ANGLE`: Terrain rotation angle

## Color Scheme

The terrain uses elevation-based coloring:

- **Snow** (< 20%): White peaks
- **Rock** (20-35%): Gray mountain slopes
- **Grass** (35-65%): Green valleys
- **Sand** (65-85%): Beige lowlands
- **Water** (> 85%): Blue sea level

## Technical Details

- **Resolution**: Configurable (default: 257x257)
- **Projection**: Isometric (30° tilt, 45° rotation)
- **Rendering**: Wireframe grid with color-coded elevation
- **Screen**: 800x700 pixels with auto-scaling

## Author

Claudio Genio

## License

This project is open source and available under the MIT License.

## Acknowledgments

- Raylib graphics library
- Diamond-Square algorithm (Fournier, Fussell, and Carpenter, 1982)
