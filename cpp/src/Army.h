#pragma once
#include "Province.h"
#include <vector>

struct UnitGroup {
    int infantry = 0;
    int armor = 0;
    int artillery = 0;
};

class Army {
public:
    int id = 0;
    Province* location = nullptr;
    UnitGroup units;
    bool isMoving = false;
    bool active = true;
    std::vector<Province*> path;
    size_t pathIndex = 0;

    void moveTo(Province* dest);
    void setPath(const std::vector<Province*>& newPath);
    void update();
    int strength() const;
    void resolveCombat(Army& enemy);
};
