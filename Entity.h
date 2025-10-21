//
// Created by Liam on 21/10/2025.
//

#ifndef CLIENT_ENTITY_H
#define CLIENT_ENTITY_H


class Entity
{
    public:
    int health;
    int speed;
    int defence;
};

class Player : Entity
{
    public:
    Player();
};


#endif //CLIENT_ENTITY_H