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

The resulting executable `OpenFrontPrototype` opens a demo window and draws a
province grid from `maps/highdensity.map`. This map contains 100×75 provinces
for a much denser play field. Each character in the file represents a terrain
type (`P` plains, `F` forest, `M` mountain, `W` water).

Run the prototype from the `build` directory:

```bash
./OpenFrontPrototype
```

Two player armies start on the map. **Left click** an army to select it, then
**right click** an adjacent province to order the selected force to move there.
Capturing a new province lowers that army’s infantry count by one. Press the
space bar to generate a path from the selected army to the blue enemy army and
watch it advance automatically. The enemy still chases the closest active
player army each tick.
