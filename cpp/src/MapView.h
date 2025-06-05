#pragma once
#include <SDL2/SDL.h>
#include "GameMap.h"

class MapView {
public:
    MapView(const GameMap& map, int windowWidth, int windowHeight, int tileSize = 2);

    void pan(int dx, int dy);
    void zoom(int delta);
    void render(SDL_Renderer* renderer);

private:
    const GameMap& map_;
    int windowWidth_;
    int windowHeight_;
    int tileSize_;
    int offsetX_;
    int offsetY_;

    void clampOffsets();
};
