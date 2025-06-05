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

province grid from `maps/gigantic.map`. This map contains 200×150 provinces
for an extremely dense play field. Each character in the file represents a terrain
One player army starts on the map. **Left click** it to select it and **right
click** an adjacent province to move. Capturing a new province lowers your
infantry count by one. There are no enemies in this demo—expand across the
gigantic map at your leisure.

```bash
./OpenFrontPrototype
```

Two player armies start on the map. **Left click** an army to select it, then
**right click** an adjacent province to order the selected force to move there.
Capturing a new province lowers that army’s infantry count by one. Press the
space bar to generate a path from the selected army to the blue enemy army and
watch it advance automatically. The enemy still chases the closest active
player army each tick.

