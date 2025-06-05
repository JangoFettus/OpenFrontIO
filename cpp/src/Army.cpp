#include "Army.h"

void Army::moveTo(Province* dest) {
    if (!dest) return;
    if (location) {
        location->occupied = false;
    }
    location = dest;
    dest->occupied = true;
    dest->owner = id;
}

void Army::setPath(const std::vector<Province*>& newPath) {
    path = newPath;
    pathIndex = 1; // first element is current position
    isMoving = pathIndex < path.size();
}

void Army::update() {
    if (!isMoving || pathIndex >= path.size()) return;
    moveTo(path[pathIndex]);
    ++pathIndex;
    if (pathIndex >= path.size()) {
        isMoving = false;
        path.clear();
    }
}

int Army::strength() const {
    return units.infantry + units.armor * 2 + units.artillery * 3;
}

void Army::resolveCombat(Army& enemy) {
    if (!active || !enemy.active) return;
    int s1 = strength();
    int s2 = enemy.strength();
    if (s1 >= s2) {
        enemy.active = false;
        if (enemy.location)
            enemy.location->occupied = false;
    } else {
        active = false;
        if (location)
            location->occupied = false;
    }
}
