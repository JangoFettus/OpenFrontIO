#include "GameMapLoader.h"
#include <stdexcept>

GameMap loadTerrainMapBin(const std::string& filename) {
    return GameMap::loadFromBin(filename);
}
