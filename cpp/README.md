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
a province grid from one of the bin maps in `../resources/maps`. Each
character in the file represents a terrain type.

Run the prototype from the `build` directory:

```bash
./OpenFrontPrototype
```

When the prototype starts you have ten seconds to click a starting tile on the
map. Your army of 500 units will spawn on that province. After placing the army
use the arrow keys to move around. Moving into a new province claims it for the
player and reduces your remaining units.

