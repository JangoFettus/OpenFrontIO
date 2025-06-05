#include <SDL2/SDL.h>
#include <iostream>
#include "Province.h"
#include "Army.h"
#include "Pathfinder.h"
#include "MapLoader.h"
#include <vector>
#include <cmath>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    ProvinceGrid provinces;
    try {
        provinces = loadMap("../maps/highdensity.map");
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        SDL_Quit();
        return 1;
    }

    const int tileSize = 8;
    int windowWidth = provinces[0].size() * tileSize;
    int windowHeight = provinces.size() * tileSize;

    SDL_Window* win = SDL_CreateWindow("OpenFront Prototype", 100, 100, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
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

    std::vector<Army> playerArmies;
    Army army1;
    army1.id = 1;
    army1.units = {100, 10, 5};
    army1.moveTo(&provinces[10][10]);
    playerArmies.push_back(army1);

    Army army2;
    army2.id = 1;
    army2.units = {80, 5, 2};
    army2.moveTo(&provinces[12][12]);
    playerArmies.push_back(army2);

    size_t selectedArmy = 0; // index into playerArmies

    Army enemy;
    enemy.id = 2;
    enemy.units = {80, 5, 0};
    enemy.moveTo(&provinces[20][15]);
    enemy.setPath(findPath(provinces, enemy.location->x, enemy.location->y,
                           playerArmies[selectedArmy].location->x, playerArmies[selectedArmy].location->y));

    // tileSize defined earlier

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
                if (p.owner == 1) {
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
        SDL_SetRenderDrawColor(ren, 220, 20, 60, 255);
        for (size_t i = 0; i < playerArmies.size(); ++i) {
            const Army& a = playerArmies[i];
            if (!a.active || !a.location) continue;
            SDL_Rect pRect{a.location->x * tileSize, a.location->y * tileSize, tileSize, tileSize};
            SDL_RenderFillRect(ren, &pRect);
            if (i == selectedArmy) {
                SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
                SDL_RenderDrawRect(ren, &pRect);
                SDL_SetRenderDrawColor(ren, 220, 20, 60, 255);
            }
        }
        SDL_RenderPresent(ren);
    };

    bool quit = false;
    SDL_Event e;
    drawMap();
    while (!quit) {
        Army& player = playerArmies[selectedArmy];
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
                            bool captured = dest.owner != player.id;
                            player.moveTo(&dest);
                            if (captured && player.units.infantry > 0) {
                                --player.units.infantry;
                            }
                            if (enemy.active && enemy.location == &dest) {
                                player.resolveCombat(enemy);
                            }
                            drawMap();
                        }
                    }
                }
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x / tileSize;
                int my = e.button.y / tileSize;
                int h = provinces.size();
                int w = provinces[0].size();
                if (mx >= 0 && mx < w && my >= 0 && my < h) {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        for (size_t i = 0; i < playerArmies.size(); ++i) {
                            if (playerArmies[i].active && playerArmies[i].location == &provinces[my][mx]) {
                                selectedArmy = i;
                                break;
                            }
                        }
                        drawMap();
                    } else if (e.button.button == SDL_BUTTON_RIGHT && player.active) {
                        int adx = std::abs(mx - player.location->x);
                        int ady = std::abs(my - player.location->y);
                        if (adx + ady == 1) {
                            Province& dest = provinces[my][mx];
                            if (dest.terrain != TerrainType::Water) {
                                bool captured = dest.owner != player.id;
                                player.moveTo(&dest);
                                if (captured && player.units.infantry > 0) {
                                    --player.units.infantry;
                                }
                                if (enemy.active && enemy.location == &dest) {
                                    player.resolveCombat(enemy);
                                }
                                drawMap();
                            }
                        }
                    }
                }
            }
        }
        for (auto& a : playerArmies) {
            a.update();
        }
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
