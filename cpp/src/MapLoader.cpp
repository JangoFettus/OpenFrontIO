#include "MapLoader.h"
#include <fstream>
#include <stdexcept>

ProvinceGrid loadMap(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open map file: " + filename);
    }

    ProvinceGrid grid;
    std::string line;
    int y = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::vector<Province> row;
        for (int x = 0; x < static_cast<int>(line.size()); ++x) {
            TerrainType terrain = TerrainType::Plains;
            switch (line[x]) {
                case 'F': terrain = TerrainType::Forest; break;
                case 'M': terrain = TerrainType::Mountain; break;
                case 'W': terrain = TerrainType::Water; break;
                case 'P': default: terrain = TerrainType::Plains; break;
            }
            row.push_back({x, y, terrain, -1, false});
        }
        grid.push_back(std::move(row));
        ++y;
    }
    return grid;
}
