//
// Created by Liam on 29/11/2025.
//

#include "Window.h"

#include "Game.h"

Window::Window() {
    // Check if we can create an SDL Window
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Start creating a window for SDL
    window = SDL_CreateWindow(
        "Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_SHOWN
    );

    // if the window creation failed, print the error and exit
    if (!window) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        windowIsOpen = false;
        return;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    // This function creates a renderer for the window, using hardware acceleration
    // and VSync
    // -1 indicates that it will use the first available rendering driver

    // If the renderer creation fails, print the error and exit
    if (!renderer) {
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError();
        SDL_DestroyWindow(window);
        SDL_Quit();
        windowIsOpen = false;
        return;
    }

    // Finally, we get to the main loop
    SDL_Event event;

    //Game game;

    while (windowIsOpen)
    {
        // Process events
        while (SDL_PollEvent(&event)) {
            getInputs(event);
            if (event.type == SDL_QUIT) {
                windowIsOpen = false;
            }
        }

        // Updating positions of objects goes here

        SDL_RenderClear(renderer);
        // Render call goes here
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return;
}

void Window::getInputs(SDL_Event &event)
{
    switch (event.key.keysym.sym)
    {
    case SDLK_w:
        keys[0] = event.type == SDL_KEYDOWN;
        break;
    case SDLK_s:
        keys[1] = event.type == SDL_KEYDOWN;
        break;
    default:
        break;
    }
    std::cout << "keys: " << keys[0] << " " << keys[1] << std::endl;
}

void Window::instantiateGameObjects()
{

}
