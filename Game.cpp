//
// Created by Liam on 21/10/2025.
//

#include "Game.h"
#include "GameObject.h"
#include <cstdio>


Game::Game(SDL_Renderer * renderer)
{
    printf("Game started");
}

void Game::createGameObjects(SDL_Renderer * renderer)
{
    printf("Creating game objects");
    player_paddle = new Paddle(renderer, "./white.png", 0, 0, 32, 32);
}

void Game::update(bool inputs[6])
{
    for (int i = 0; i < 6; i++)
    {
        switch (i)
        {
        case 3:
            if (inputs[i])
            {
                player_paddle->setX(player_paddle->getX() + 1);
            }
            break;
        case 1:
            if (inputs[i])
            {
                player_paddle->setX(player_paddle->getX() - 1);
            }
            break;
        case 2:
            if (inputs[i])
            {
                player_paddle->setY(player_paddle->getY() + 1);
            }
            break;
        case 0:
            if (inputs[i])
            {
                player_paddle->setY(player_paddle->getY() - 1);
            }
            break;
        default:
            break;
        }
    }

    player_paddle->update();
}

void Game::render(SDL_Renderer * renderer)
{
    player_paddle->render(renderer);
}
