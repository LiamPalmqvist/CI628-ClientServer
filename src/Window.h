//
// Created by Liam on 29/11/2025.
//

#ifndef CLIENT_SDL_WINDOW_H
#define CLIENT_SDL_WINDOW_H

#include <SDL2/SDL.h>
#include <iostream>

class Window
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    void getInputs(SDL_Event &event);
public:
    // Since we only need two keys (up and down)
    //              W      S
    bool keys[2] = {false, false};
    bool windowIsOpen = true;
    Window();
};


#endif //CLIENT_SDL_WINDOW_H