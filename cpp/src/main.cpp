#include <SDL2/SDL.h>
#include <iostream>
#include "GameMap.h"
#include "GameMapLoader.h"
#include "MapView.h"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    GameMap map = GameMap::loadFromBin("../resources/maps/PangaeaMini.bin");

    const int windowWidth = 800;
    const int windowHeight = 600;


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

    MapView view(map, windowWidth, windowHeight, 2);
    view.render(ren);


    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_EQUALS:
                    case SDLK_PLUS:
                        view.zoom(1);
                        break;
                    case SDLK_MINUS:
                        view.zoom(-1);
                        break;
                    case SDLK_UP:
                        view.pan(0, -1);
                        break;
                    case SDLK_DOWN:
                        view.pan(0, 1);
                        break;
                    case SDLK_LEFT:
                        view.pan(-1, 0);
                        break;
                    case SDLK_RIGHT:
                        view.pan(1, 0);
                        break;
                }
            }
        }
        view.render(ren);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
