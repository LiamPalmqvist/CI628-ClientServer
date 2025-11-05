//
// Created by Liam on 21/10/2025.
//

#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H
#include "GameObject.h"

class Game
{
public:
    Game(SDL_Renderer *renderer);
    void createGameObjects(SDL_Renderer * renderer);
    void update(bool inputs[6]);
    void render(SDL_Renderer * renderer);

    Paddle *player_paddle;
    //Paddle* player_paddle2;

    //Ball* ball;
};

#endif