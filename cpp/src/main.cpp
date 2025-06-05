#include <SDL2/SDL.h>
#include <iostream>
#include "GameMap.h"
#include "GameMapLoader.h"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    GameMap map = GameMap::loadFromBin("../resources/maps/PangaeaMini.bin");

    const int tileSize = 16; // larger tiles so we can see clicking locations
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

    GameMap::TileRef playerPos = 0;
    int playerUnits = 0;
    bool spawnPlaced = false;
    Uint32 startTime = SDL_GetTicks();

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
            if (map.ownerID(r) == 1) {
                SDL_SetRenderDrawColor(ren, 192, 64, 64, 120);
                SDL_RenderFillRect(ren, &rect);
            }
        }
        if (spawnPlaced) {
            SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
            SDL_Rect army{map.x(playerPos) * tileSize, map.y(playerPos) * tileSize,
                         tileSize, tileSize};
            SDL_RenderFillRect(ren, &army);
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
            } else if (!spawnPlaced && e.type == SDL_MOUSEBUTTONDOWN &&
                       e.button.button == SDL_BUTTON_LEFT) {
                Uint32 elapsed = SDL_GetTicks() - startTime;
                if (elapsed < 10000) {
                    int mx = e.button.x / tileSize;
                    int my = e.button.y / tileSize;
                    if (map.isValidCoord(mx, my)) {
                        auto ref = map.ref(mx, my);
                        if (map.isLand(ref)) {
                            spawnPlaced = true;
                            playerPos = ref;
                            playerUnits = 500;
                            map.setOwnerID(ref, 1);
                            std::cout << "Spawn placed at (" << mx << "," << my
                                      << ") with 500 units\n";
                        }
                    }
                }
            } else if (spawnPlaced && e.type == SDL_KEYDOWN) {
                int dx = 0, dy = 0;
                switch (e.key.keysym.sym) {
                    case SDLK_UP:    dy = -1; break;
                    case SDLK_DOWN:  dy = 1;  break;
                    case SDLK_LEFT:  dx = -1; break;
                    case SDLK_RIGHT: dx = 1;  break;
                    default: break;
                }
                if (dx != 0 || dy != 0) {
                    int nx = map.x(playerPos) + dx;
                    int ny = map.y(playerPos) + dy;
                    if (map.isValidCoord(nx, ny)) {
                        auto next = map.ref(nx, ny);
                        if (map.isLand(next)) {
                            playerPos = next;
                            if (map.ownerID(next) != 1 && playerUnits > 0) {
                                --playerUnits;
                            }
                            map.setOwnerID(next, 1);
                            std::string title = "Units: " + std::to_string(playerUnits);
                            SDL_SetWindowTitle(win, title.c_str());
                        }
                    }
                }
            }
        }
        drawMap();
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
