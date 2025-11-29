//
// Created by Liam on 25/11/2025.
//

#include "Game.h"
#include <ostream>


Paddle::Paddle(const int player)
{
    if (player == 0)
        _xPos = 30;
    else
        _xPos = 530;
    _yPos = 300;
    _xSize = 10;
    _ySize = 30;
}

Paddle::Paddle(const int xPos, const int yPos, const int xSize, const int ySize)
{
    _xPos = xPos;
    _yPos = yPos;
    _xSize = xSize;
    _ySize = ySize;
}

Ball::Ball()
{
    _xPos = 300;
    _yPos = 200;
    _radius = 10;
    _direction = 20;
}

Ball::Ball(const int xPos, const int yPos, const int size)
{
    _xPos = xPos;
    _yPos = yPos;
    _radius = size;
    _direction = 20;
}

Game::Game() : p1Paddle(0), p2Paddle(1), ball()
{
    playing = true;
}

void Game::update(const bool p1keys[6], const bool p2keys[6])
{
    // First, we check how many points each player has
    // if either player has 10 points, end the game
    if (p1Points >= 10)
    {
        playing = false;
        return;
    }
    if (p2Points >= 10)
    {
        playing = false;
        return;
    }


    // Second, we need to actually check where the ball is
    // Saves us updating things unnecessarily
    const int direction = ball.get_direction();
    const int speed = ball.get_speed();
    const int x = ball.get_x_pos();
    const int y = ball.get_y_pos();

    if (x < 80)
    {
        p1Points++;
        ball.set_x_pos(200);
        ball.set_y_pos(300);
        ball.set_direction(20);
        p1Paddle.set_y_pos(300);
        p2Paddle.set_y_pos(300);
        return;
    }
    if (x > 320)
    {
        p2Points++;
        ball.set_x_pos(200);
        ball.set_y_pos(300);
        p1Paddle.set_y_pos(300);
        p2Paddle.set_y_pos(300);
        return;
    }

    // get information about the paddles
    const int p1y = p1Paddle.get_y_pos();
    const int p1ySize = p1Paddle.get_ySize();

    const int p2y = p2Paddle.get_y_pos();
    const int p2ySize = p2Paddle.get_ySize();

    // Third, calculate the players' movements
    if (p1keys[0])
    {
        if (p1y + p1ySize/2 > 0)
            p1Paddle.set_y_pos(p1y + 1);
    }
    if (p1keys[1])
    {
        if (p1y + p1ySize/2 < 600)
            p1Paddle.set_y_pos(p1y - 1);
    }
    if (p2keys[0])
    {
        if (p1y + p1ySize/2 > 0)
            p2Paddle.set_y_pos(p2y + 1);
    }
    if (p2keys[1])
    {
        if (p2y + p2ySize/2 < 600)
            p2Paddle.set_y_pos(p2y - 1);
    }

    // Last, calculate ball's movement using trig
    // the speed will act as the hypotenuse and the direction the angle
    // if we have the hypotenuse and the angle, we can use the equation
    // hypotenuse * cos(angle) to get the tangent (x)
    // hypotenuse * sin(angle) to get the opposite (y)
    // the opposite will be our y value
    // the tangent will be our x value
    const float dx = speed * cos(direction);
    const float dy = speed * sin(direction);
    p1Paddle.set_x_pos(x+dx);
    p1Paddle.set_y_pos(y+dy);

    checkCollisions();
}

void Game::checkCollisions()
{
    const int ballX = ball.get_x_pos();
    const int ballY = ball.get_y_pos();
    int rotation = ball.get_direction();

    // get information about the paddles
    const int p1x = p1Paddle.get_x_pos();
    const int p1y = p1Paddle.get_y_pos();
    const int p1xSize = p1Paddle.get_xSize();
    const int p1ySize = p1Paddle.get_ySize();

    const int p2x = p2Paddle.get_x_pos();
    const int p2y = p2Paddle.get_y_pos();
    const int p2xSize = p2Paddle.get_xSize();
    const int p2ySize = p2Paddle.get_ySize();

    // check if the ball is within the bounds of the paddles
    if (ballX < p1x + p1xSize/2 && ballX > p1x - p1xSize/2 && ballY < p1y + p1ySize/2 && ballY > p1y + p1ySize/2)
    {
        rotation += 180;
        if (rotation > 360)
            rotation-=360;
        else if (rotation < 0)
            rotation+=360;
        ball.set_direction(rotation);
    }
    else if (ballX < p2x + p2xSize/2 && ballX > p2x - p2xSize/2 && ballY < p2y + p2ySize/2 && ballY > p2y + p2ySize/2)
    {
        rotation += 180;
        if (rotation > 360)
            rotation-=360;
        else if (rotation < 0)
            rotation+=360;
        ball.set_direction(rotation);
    }
}

std::string Game::encodeData()
{
    // get information about the paddles
    const std::string p1x = std::to_string(p1Paddle.get_x_pos());
    const std::string p1y = std::to_string(p1Paddle.get_y_pos());
    const std::string p1speed = std::to_string(p1Paddle.get_speed());
    const std::string p1xSize = std::to_string(p1Paddle.get_xSize());
    const std::string p1ySize = std::to_string(p1Paddle.get_ySize());

    const std::string p2x = std::to_string(p2Paddle.get_x_pos());
    const std::string p2y = std::to_string(p2Paddle.get_y_pos());
    const std::string p2speed = std::to_string(p2Paddle.get_speed());
    const std::string p2xSize = std::to_string(p2Paddle.get_xSize());
    const std::string p2ySize = std::to_string(p2Paddle.get_ySize());

    const std::string ballX = std::to_string(ball.get_x_pos());
    const std::string ballY = std::to_string(ball.get_y_pos());
    const std::string ballSpeed = std::to_string(ball.get_speed());
    const std::string ballRadius = std::to_string(ball.get_radius());
    const std::string rotation = std::to_string(ball.get_direction());

    std::string jsonTemplate = "{"
        "\"p1\" : {"
        "\"xpos\" : " + p1x + ","
        "\"ypos\" : " + p1y + ","
        "\"speed\" : " + p1speed + ","
        "\"sizex\" : " + p1xSize + ","
        "\"sizey\" : " + p1ySize + ","
        "\"points\" : " + std::to_string(p1Points) +
        "},"
        "\"p2\" : {"
        "\"xpos\" : " + p2x + ","
        "\"ypos\" : " + p2y + ","
        "\"speed\" : " + p2speed + ","
        "\"sizex\" : " + p2xSize + ","
        "\"sizey\" : " + p2ySize + ","
        "\"points\" : " + std::to_string(p2Points) +
        "},"
        "\"ball\" : {"
        "\"xpos\" : " + ballX + ","
        "\"ypos\" : " + ballY + ","
        "\"speed\" : " + ballSpeed + ","
        "\"radius\" : " + ballRadius + ","
        "\"direction\" : " + rotation +
        "}"
        "}";

    return jsonTemplate;
}