//
// Created by Liam on 05/11/2025.
//

#ifndef CLIENT_GAMEOBJECT_H
#define CLIENT_GAMEOBJECT_H

#include <SDL_image.h>

class GameObject
{
public:
    GameObject();
    GameObject(SDL_Renderer * renderer, const char* spriteFileName, float xPos, float yPos, int width, int height);
    void render(SDL_Renderer * renderer) const;
    void LoadTexture(const char* spriteFilename, SDL_Renderer * renderer);
    void update();
    float getX() const { return _x; }
    float getY() const { return _y; }
    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
    int getAngle() const { return _angle; }
    void setX(const int x) { _x = x; }
    void setY(const int y) { _y = y; }
    void setWidth(const int width) { _width = width; }
    void setHeight(const int height) { _height = height; }
    void setAngle(const int angle) { _angle = angle; }

protected:
    // general information
    float _x, _y;
    int _width, _height, _angle;

    // SDL specific information
    SDL_Texture* spriteTexture = nullptr;
    SDL_Rect srcRect = { 0, 0, 0, 0 };
    SDL_Rect destRect = { 0, 0, 0, 0 };
};

class Paddle : GameObject
{
public:
    using GameObject::GameObject;
    using GameObject::render;
    using GameObject::update;

    using GameObject::getX;
    using GameObject::getY;
    using GameObject::getWidth;
    using GameObject::getHeight;
    using GameObject::getAngle;

    using GameObject::setX;
    using GameObject::setY;
    using GameObject::setWidth;
    using GameObject::setHeight;
    using GameObject::setAngle;
};

#endif //CLIENT_GAMEOBJECT_H