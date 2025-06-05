#include <SDL2/SDL.h>
#include <iostream>
#include "Province.h"
#include "Army.h"
#include "Pathfinder.h"
#include "MapLoader.h"
#include <vector>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("OpenFront Prototype", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    if (!win) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
        SDL_DestroyWindow(win);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    ProvinceGrid provinces;
    try {
        provinces = loadMap("../maps/sample.map");
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    Army player;
    player.id = 1;
    player.units = {100, 10, 5};
    player.moveTo(&provinces[2][2]);

    Army enemy;
    enemy.id = 2;
    enemy.units = {80, 5, 0};
    enemy.moveTo(&provinces[4][4]);
    enemy.setPath(findPath(provinces, enemy.location->x, enemy.location->y,
                           player.location->x, player.location->y));

    const int tileSize = 32;

    auto drawMap = [&]() {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        for (size_t y = 0; y < provinces.size(); ++y) {
            for (size_t x = 0; x < provinces[y].size(); ++x) {
                const Province& p = provinces[y][x];
                switch (p.terrain) {
                    case TerrainType::Forest:   SDL_SetRenderDrawColor(ren, 0, 80, 0, 255); break;
                    case TerrainType::Mountain: SDL_SetRenderDrawColor(ren, 160, 160, 160, 255); break;
                    case TerrainType::Water:    SDL_SetRenderDrawColor(ren, 64, 64, 224, 255); break;
                    case TerrainType::Plains:
                    default:                    SDL_SetRenderDrawColor(ren, 136, 192, 80, 255); break;
                }
                SDL_Rect r{static_cast<int>(x) * tileSize, static_cast<int>(y) * tileSize, tileSize, tileSize};
                SDL_RenderFillRect(ren, &r);
                if (p.owner == player.id) {
                    SDL_SetRenderDrawColor(ren, 220, 20, 60, 100);
                    SDL_RenderFillRect(ren, &r);
                } else if (p.owner == enemy.id) {
                    SDL_SetRenderDrawColor(ren, 20, 20, 220, 100);
                    SDL_RenderFillRect(ren, &r);
                }
                SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
                SDL_RenderDrawRect(ren, &r);
            }
        }
        if (enemy.active) {
            SDL_SetRenderDrawColor(ren, 20, 20, 220, 255);
            SDL_Rect eRect{enemy.location->x * tileSize, enemy.location->y * tileSize, tileSize, tileSize};
            SDL_RenderFillRect(ren, &eRect);
        }
        if (player.active) {
            SDL_SetRenderDrawColor(ren, 220, 20, 60, 255);
            SDL_Rect pRect{player.location->x * tileSize, player.location->y * tileSize, tileSize, tileSize};
            SDL_RenderFillRect(ren, &pRect);
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
            } else if (e.type == SDL_KEYDOWN && player.active) {
                int dx = 0, dy = 0;
                switch (e.key.keysym.sym) {
                    case SDLK_UP: dy = -1; break;
                    case SDLK_DOWN: dy = 1; break;
                    case SDLK_LEFT: dx = -1; break;
                    case SDLK_RIGHT: dx = 1; break;
                    case SDLK_SPACE: {
                        auto path = findPath(provinces, player.location->x, player.location->y,
                                              enemy.location->x, enemy.location->y);
                        player.setPath(path);
                        break;
                    }
                    default: break;
                }
                if (dx != 0 || dy != 0) {
                    int newX = player.location->x + dx;
                    int newY = player.location->y + dy;
                    int h = provinces.size();
                    int w = provinces[0].size();
                    if (newX >= 0 && newX < w && newY >= 0 && newY < h) {
                        Province& dest = provinces[newY][newX];
                        if (dest.terrain != TerrainType::Water) {
                            player.moveTo(&dest);
                            if (enemy.active && enemy.location == &dest) {
                                player.resolveCombat(enemy);
                            }
                            drawMap();
                        }
                    }
                }
            }
        }
        player.update();
        enemy.update();
        if (!enemy.isMoving && enemy.active && player.active) {
            auto path = findPath(provinces, enemy.location->x, enemy.location->y,
                                 player.location->x, player.location->y);
            enemy.setPath(path);
        }
        if (player.active && enemy.active && player.location == enemy.location) {
            player.resolveCombat(enemy);
        }
        drawMap();
        SDL_Delay(100);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
