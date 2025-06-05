# OpenFront C++ Prototype

This directory contains a simple SDL-based prototype for a future C++ version of OpenFront.

## Building

Ensure you have `SDL2` and `CMake` installed, then run:

```bash
mkdir build
cd build
cmake ..
make
```

The resulting executable `OpenFrontPrototype` opens a small demo window and draws
a province grid from `maps/sample.map`. Each character in the file represents a
terrain type (`P` plains, `F` forest, `M` mountain, `W` water).

Run the prototype from the `build` directory:

```bash
./OpenFrontPrototype
```

Use the arrow keys to move the red player army around the map. Provinces you
enter become owned by the player and are tinted red. Press the space bar to
generate a path to the blue enemy army and watch your units move automatically.
The enemy now has a simple AI: if it is active, it will plot a path towards the
player and chase them each tick.
