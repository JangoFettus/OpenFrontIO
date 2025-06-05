#pragma once
#include "Province.h"
#include "Pathfinder.h" // for ProvinceGrid
#include <string>
#include <vector>

ProvinceGrid loadMap(const std::string& filename);
