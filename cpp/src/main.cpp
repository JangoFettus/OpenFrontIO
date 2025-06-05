#include <SDL2/SDL.h>
#include <iostream>
#include "GameMap.h"
#include "GameMapLoader.h"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // When running from the cpp/build directory the project root is two levels
    // up, so load the binary map relative to that location. Using a fixed
    // relative path keeps things simple for this prototype.
    const char* mapPath = "../../resources/maps/PangaeaMini.bin";
    if (argc > 1) {
        mapPath = argv[1];
    }

    GameMap map = GameMap::loadFromBin(mapPath);

    const int tileSize = 2;
    int windowWidth = map.width() * tileSize;
    int windowHeight = map.height() * tileSize;

    SDL_Window* win = SDL_CreateWindow("OpenFront Prototype", 100, 100,
                                       windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!win) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
        SDL_DestroyWindow(win);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    auto drawMap = [&]() {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        int total = map.width() * map.height();
        for (int r = 0; r < total; ++r) {
            switch (map.terrainType(r)) {
                case TerrainType::Plains:
                    SDL_SetRenderDrawColor(ren, 136, 192, 80, 255);
                    break;
                case TerrainType::Highland:
                    SDL_SetRenderDrawColor(ren, 160, 160, 80, 255);
                    break;
                case TerrainType::Mountain:
                    SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
                    break;
                case TerrainType::Lake:
                    SDL_SetRenderDrawColor(ren, 64, 160, 224, 255);
                    break;
                case TerrainType::Ocean:
                default:
                    SDL_SetRenderDrawColor(ren, 0, 96, 192, 255);
                    break;
            }
            SDL_Rect rect{map.x(r) * tileSize, map.y(r) * tileSize,
                          tileSize, tileSize};
            SDL_RenderFillRect(ren, &rect);
        }
        SDL_RenderPresent(ren);
    };

    bool quit = false;
    SDL_Event e;
    drawMap();
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
