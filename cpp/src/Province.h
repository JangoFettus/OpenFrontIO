#pragma once
#include <cstdint>

enum class TerrainType {
    Plains,
    Forest,
    Mountain,
    Water
};

struct Province {
    int x;
    int y;
    TerrainType terrain;
    int owner; // -1 = neutral
    bool occupied;
};
