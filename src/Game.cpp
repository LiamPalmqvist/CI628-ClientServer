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
    _speed = 10;
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
    _speed = 10;
}

Ball::Ball(const int xPos, const int yPos, const int size)
{
    _xPos = xPos;
    _yPos = yPos;
    _radius = size;
    _direction = 20;
    _speed = 10;
}

Game::Game() : ball()
{
    playing = false;
}

// void Game::update(const bool p1keys[2], const bool p2keys[2])
// {
//     // First, we check how many points each player has
//     // if either player has 10 points, end the game
//     if (p1Points >= 10)
//     {
//         playing = false;
//         return;
//     }
//     if (p2Points >= 10)
//     {
//         playing = false;
//         return;
//     }
//
//
//     // Second, we need to actually check where the ball is
//     // Saves us updating things unnecessarily
//     const int direction = ball.get_direction();
//     const int speed = ball.get_speed();
//     const int x = ball.get_x_pos();
//     const int y = ball.get_y_pos();
//
//     if (x < 80)
//     {
//         p1Points++;
//         ball.set_x_pos(200);
//         ball.set_y_pos(300);
//         ball.set_direction(20);
//         playerPaddles[0].set_y_pos(300);
//         playerPaddles[1].set_y_pos(300);
//         return;
//     }
//     if (x > 320)
//     {
//         p2Points++;
//         ball.set_x_pos(200);
//         ball.set_y_pos(300);
//         playerPaddles[0].set_y_pos(300);
//         playerPaddles[1].set_y_pos(300);
//         return;
//     }
//
//     // get information about the paddles
//     const int p1y = playerPaddles[0].get_y_pos();
//     const int p1ySize = playerPaddles[0].get_ySize();
//
//     const int p2y = playerPaddles[1].get_y_pos();
//     const int p2ySize = playerPaddles[1].get_ySize();
//
//     // Third, calculate the players' movements
//     if (p1keys[0])
//     {
//         if (p1y + p1ySize/2 > 0)
//             playerPaddles[0].set_y_pos(p1y + 1);
//     }
//     if (p1keys[1])
//     {
//         if (p1y + p1ySize/2 < 600)
//             playerPaddles[0].set_y_pos(p1y - 1);
//     }
//     if (p2keys[0])
//     {
//         if (p1y + p1ySize/2 > 0)
//             playerPaddles[1].set_y_pos(p2y + 1);
//     }
//     if (p2keys[1])
//     {
//         if (p2y + p2ySize/2 < 600)
//             playerPaddles[1].set_y_pos(p2y - 1);
//     }
//
//     // Last, calculate ball's movement using trig
//     // the speed will act as the hypotenuse and the direction the angle
//     // if we have the hypotenuse and the angle, we can use the equation
//     // hypotenuse * cos(angle) to get the tangent (x)
//     // hypotenuse * sin(angle) to get the opposite (y)
//     // the opposite will be our y value
//     // the tangent will be our x value
//     const float dx = speed * cos(direction);
//     const float dy = speed * sin(direction);
//     playerPaddles[0].set_x_pos(x+dx);
//     playerPaddles[0].set_y_pos(y+dy);
//
//     checkCollisions();
// }

void Game::update()
{
    std::cout << playerKeys[0] << " . " << playerKeys[1] << std::endl;
    if (playerKeys[0])
    {
        std::cout << "was " << playerPaddles[0].get_y_pos() << std::endl;
        playerPaddles[0].set_y_pos(playerPaddles[0].get_y_pos()-playerPaddles[0].get_speed());
        std::cout << "is now " << playerPaddles[0].get_y_pos() << std::endl;
    }
    if (playerKeys[1])
    {
        std::cout << "was " << playerPaddles[0].get_y_pos() << std::endl;
        playerPaddles[0].set_y_pos(playerPaddles[0].get_y_pos()+playerPaddles[0].get_speed());
        std::cout << "is now " << playerPaddles[0].get_y_pos() << std::endl;
    }
    if (playerKeys[2])
    {
        std::cout << "was " << playerPaddles[1].get_y_pos() << std::endl;
        playerPaddles[1].set_y_pos(playerPaddles[1].get_y_pos()-playerPaddles[1].get_speed());
        std::cout << "is now " << playerPaddles[1].get_y_pos() << std::endl;
    }
    if (playerKeys[3])
    {
        std::cout << "was " << playerPaddles[1].get_y_pos() << std::endl;
        playerPaddles[1].set_y_pos(playerPaddles[1].get_y_pos()+playerPaddles[1].get_speed());
        std::cout << "is now " << playerPaddles[1].get_y_pos() << std::endl;
    }
}

void Game::checkCollisions()
{
    const int ballX = ball.get_x_pos();
    const int ballY = ball.get_y_pos();
    int rotation = ball.get_direction();

    // get information about the paddles
    const int p1x = playerPaddles[0].get_x_pos();
    const int p1y = playerPaddles[0].get_y_pos();
    const int p1xSize = playerPaddles[0].get_xSize();
    const int p1ySize = playerPaddles[0].get_ySize();

    const int p2x = playerPaddles[1].get_x_pos();
    const int p2y = playerPaddles[1].get_y_pos();
    const int p2xSize = playerPaddles[1].get_xSize();
    const int p2ySize = playerPaddles[1].get_ySize();

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

int* Game::encodeData()
{
    // get information about the paddles
    int p1x = playerPaddles[0].get_x_pos();
    int p1y = playerPaddles[0].get_y_pos();
    int p1speed = playerPaddles[0].get_speed();
    int p1xSize = playerPaddles[0].get_xSize();
    int p1ySize = playerPaddles[0].get_ySize();

    int p2x = playerPaddles[1].get_x_pos();
    int p2y = playerPaddles[1].get_y_pos();
    int p2speed = playerPaddles[1].get_speed();
    int p2xSize = playerPaddles[1].get_xSize();
    int p2ySize = playerPaddles[1].get_ySize();

    int ballX = ball.get_x_pos();
    int ballY = ball.get_y_pos();
    int ballSpeed = ball.get_speed();
    int ballRadius = ball.get_radius();
    int ballRotation = ball.get_direction();

    /*std::string jsonTemplate = "{"
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
*/
    static int data[17];

    // Assign values every call
    data[0]  = p1x;
    data[1]  = p1y;
    data[2]  = p1speed;
    data[3]  = p1xSize;
    data[4]  = p1ySize;
    data[5]  = p1Points;
    data[6]  = p2x;
    data[7]  = p2y;
    data[8]  = p2speed;
    data[9]  = p2xSize;
    data[10] = p2ySize;
    data[11] = p2Points;
    data[12] = ballX;
    data[13] = ballY;
    data[14] = ballSpeed;
    data[15] = ballRadius;
    data[16] = ballRotation;

    return data;
}

void Game::decodeData(int* data)
{
    // for (int i = 0; i < 17; i++)
    // {
    //     std::cout << std::to_string(data[i]) + ", ";
    // } std::cout << std::endl;

    playerPaddles[0].set_x_pos(data[0]);
    playerPaddles[0].set_y_pos(data[1]);
    playerPaddles[0].set_speed(data[2]);
    playerPaddles[0].set_xSize(data[3]);
    playerPaddles[0].set_ySize(data[4]);
    p1Points = data[5];

    playerPaddles[1].set_x_pos(data[6]);
    playerPaddles[1].set_y_pos(data[7]);
    playerPaddles[1].set_speed(data[8]);
    playerPaddles[1].set_xSize(data[9]);
    playerPaddles[1].set_ySize(data[10]);
    p2Points = data[11];

    ball.set_x_pos(data[12]);
    ball.set_y_pos(data[13]);
    ball.set_speed(data[14]);
    ball.set_radius(data[15]);
    ball.set_direction(data[16]);
}

void Game::printData()
{
    // get information about the paddles
    const std::string p1x = std::to_string(playerPaddles[0].get_x_pos());
    const std::string p1y = std::to_string(playerPaddles[0].get_y_pos());
    const std::string p1speed = std::to_string(playerPaddles[0].get_speed());
    const std::string p1xSize = std::to_string(playerPaddles[0].get_xSize());
    const std::string p1ySize = std::to_string(playerPaddles[0].get_ySize());
    const std::string p2x = std::to_string(playerPaddles[1].get_x_pos());
    const std::string p2y = std::to_string(playerPaddles[1].get_y_pos());
    const std::string p2speed = std::to_string(playerPaddles[1].get_speed());
    const std::string p2xSize = std::to_string(playerPaddles[1].get_xSize());
    const std::string p2ySize = std::to_string(playerPaddles[1].get_ySize());
    const std::string ballX = std::to_string(ball.get_x_pos());
    const std::string ballY = std::to_string(ball.get_y_pos());
    const std::string ballSpeed = std::to_string(ball.get_speed());
    const std::string ballRadius = std::to_string(ball.get_radius());
    const std::string ballRotation = std::to_string(ball.get_direction());

    std::string jsonTemplate = "{\n"
        "\t\"p1\" : {\n"
        "\t\t\"xpos\" : " + p1x + ",\n"
        "\t\t\"ypos\" : " + p1y + ",\n"
        "\t\t\"speed\" : " + p1speed + ",\n"
        "\t\t\"sizex\" : " + p1xSize + ",\n"
        "\t\t\"sizey\" : " + p1ySize + ",\n"
        "\t\t\"points\" : " + std::to_string(p1Points) + "\n" +
        "\t},\n"
        "\t\"p2\" : {\n"
        "\t\t\"xpos\" : " + p2x + ",\n"
        "\t\t\"ypos\" : " + p2y + ",\n"
        "\t\t\"speed\" : " + p2speed + ",\n"
        "\t\t\"sizex\" : " + p2xSize + ",\n"
        "\t\t\"sizey\" : " + p2ySize + ",\n"
        "\t\t\"points\" : " + std::to_string(p2Points) + "\n" +
        "\t},\n"
        "\t\"ball\" : {\n"
        "\t\t\"xpos\" : " + ballX + ",\n"
        "\t\t\"ypos\" : " + ballY + ",\n"
        "\t\t\"speed\" : " + ballSpeed + ",\n"
        "\t\t\"radius\" : " + ballRadius + ",\n"
        "\t\t\"direction\" : " + ballRotation + + "\n" +
        "\t}\n"
        "}\n";

    std::cout << jsonTemplate << std::endl;
}