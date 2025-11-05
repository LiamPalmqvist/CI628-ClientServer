//
// Created by Liam on 05/11/2025.
//

#include "GameObject.h"

#include "Client.h"
#include "Game.h"

GameObject::GameObject() = default;

GameObject::GameObject(SDL_Renderer * renderer, const char* spriteFileName, const float xPos, const float yPos, const int width, const int height)
{

    LoadTexture(spriteFileName, renderer);

    _x = xPos;
    _y = yPos;
    srcRect.w = width;
    srcRect.h = height;
    srcRect.x = srcRect.y = 0;
    destRect.w = width;
    destRect.h = height;
    destRect.x = srcRect.x;
    destRect.y = srcRect.y;
}

void GameObject::LoadTexture(const char* spriteFileName, SDL_Renderer * renderer)
{
    SDL_Surface* surface = IMG_Load(spriteFileName);
    spriteTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (spriteTexture == nullptr) printf("Image Load Error - SDL Error: %s\n", SDL_GetError());
    SDL_FreeSurface(surface);
}

void GameObject::update()
{
    destRect.x = _x;
    destRect.y = _y;
}

void GameObject::render(SDL_Renderer * renderer) const
{
    SDL_RenderCopyEx(renderer, spriteTexture, &srcRect, &destRect, 0, NULL, SDL_FLIP_NONE);
}
