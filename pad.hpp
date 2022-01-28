#pragma once

#include <SFML/Graphics.hpp>

class Ball;

#include "ball.hpp"

class Pad {
    const float
        INIT_X_SPACING = 100.f,
        INIT_Y = 300.f,
        SPEED = 0.2f;

    const sf::Color
        COLOR_A = sf::Color::Blue,
        COLOR_B = sf::Color::Yellow;

    float screenLowerPoint = 0; // for limiting movement

    // to be called every time we update the position
    void updateRenderPos();

public:
    const float SIZE_X = 30.f,
        SIZE_Y = 150.f;

    float x, y;
    sf::RectangleShape renderObj;

    Pad(float screenWidth, float screenHeight, bool isPadA);
    void goUp();
    void goDown();
    void followBall(Ball* ball);
    void reset();
};