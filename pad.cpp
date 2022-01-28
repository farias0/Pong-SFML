#include <SFML/Graphics.hpp>

class Ball;

#include "pad.hpp"
#include "ball.hpp"

void Pad::updateRenderPos() {
	this->renderObj.setPosition(sf::Vector2f(x, y));
}

Pad::Pad(float screenWidth, float screenHeight, bool isPadA) {
    x = isPadA ? INIT_X_SPACING : screenWidth - INIT_X_SPACING;
    y = INIT_Y;
    screenLowerPoint = screenHeight;

    // create render obj
    this->renderObj = sf::RectangleShape();
    this->renderObj.setSize(sf::Vector2f(SIZE_X, SIZE_Y));
    this->renderObj.setFillColor(isPadA ? COLOR_A : COLOR_B);
    updateRenderPos();
}

void Pad::goUp() {
    float newPos = y - SPEED;
    if (newPos > 0) {
        y = newPos;
    }
    updateRenderPos();
}

void Pad::goDown() {
    float newPos = y + SPEED;
    if (newPos + SIZE_Y < screenLowerPoint) {
        y = newPos;
    }
    updateRenderPos();
}

void Pad::followBall(Ball* ball) {
    y = ball->y - (SIZE_Y / 2);
    if (y < 0) {
        y = 0;
    }
    updateRenderPos();
}

void Pad::reset() {
    y = INIT_Y;
    updateRenderPos();
}