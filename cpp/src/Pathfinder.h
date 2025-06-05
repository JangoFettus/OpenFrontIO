#pragma once
#include "Province.h"
#include <vector>

using ProvinceGrid = std::vector<std::vector<Province>>;

std::vector<Province*> findPath(ProvinceGrid& grid, int startX, int startY, int goalX, int goalY);
