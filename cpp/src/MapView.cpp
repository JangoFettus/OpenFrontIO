#include "MapView.h"
#include <algorithm>

MapView::MapView(const GameMap& map, int windowWidth, int windowHeight, int tileSize)
    : map_(map), windowWidth_(windowWidth), windowHeight_(windowHeight),
      tileSize_(tileSize), offsetX_(0), offsetY_(0) {}

void MapView::clampOffsets() {
    int maxX = std::max(0, map_.width() - windowWidth_ / tileSize_);
    int maxY = std::max(0, map_.height() - windowHeight_ / tileSize_);
    if (offsetX_ < 0) offsetX_ = 0;
    if (offsetY_ < 0) offsetY_ = 0;
    if (offsetX_ > maxX) offsetX_ = maxX;
    if (offsetY_ > maxY) offsetY_ = maxY;
}

void MapView::pan(int dx, int dy) {
    offsetX_ += dx;
    offsetY_ += dy;
    clampOffsets();
}

void MapView::zoom(int delta) {
    int newSize = tileSize_ + delta;
    newSize = std::clamp(newSize, 1, 64);
    if (newSize != tileSize_) {
        tileSize_ = newSize;
        clampOffsets();
    }
}

void MapView::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    int tilesX = windowWidth_ / tileSize_;
    int tilesY = windowHeight_ / tileSize_;
    for (int y = 0; y < tilesY; ++y) {
        int mapY = y + offsetY_;
        if (mapY >= map_.height()) break;
        for (int x = 0; x < tilesX; ++x) {
            int mapX = x + offsetX_;
            if (mapX >= map_.width()) break;
            GameMap::TileRef r = map_.ref(mapX, mapY);
            switch (map_.terrainType(r)) {
                case TerrainType::Plains:
                    SDL_SetRenderDrawColor(renderer, 136, 192, 80, 255);
                    break;
                case TerrainType::Highland:
                    SDL_SetRenderDrawColor(renderer, 160, 160, 80, 255);
                    break;
                case TerrainType::Mountain:
                    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
                    break;
                case TerrainType::Lake:
                    SDL_SetRenderDrawColor(renderer, 64, 160, 224, 255);
                    break;
                case TerrainType::Ocean:
                default:
                    SDL_SetRenderDrawColor(renderer, 0, 96, 192, 255);
                    break;
            }
            SDL_Rect rect{x * tileSize_, y * tileSize_, tileSize_, tileSize_};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}
