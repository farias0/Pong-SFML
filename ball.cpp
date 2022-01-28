#include <SFML/Graphics.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <random>

#include "ball.hpp"

using namespace std;

void Ball::updateRenderPos() {
	this->renderObj.setPosition(sf::Vector2f(x, y));
}

float Ball::calcLaunchAngle() {
	float a = 0.f;

	// TODO do this without a loop
	while (a == 0) { // make sure float 0 == int 0
		float rand = (float) this->distr(this->gen); // random float from 0 to 2 * PI

		// cout << "\ncalculated launch angle: " << rand;

		if (rand < LAUNCH_LEFT_UPPER_BOUND || // from 0 to the 1st bound (half the right cone)
			(rand > LAUNCH_RIGHT_UPPER_BOUND && rand <= LAUNCH_RIGHT_LOWER_BOUND) || // from the 2nd to the 3rd bound (the left cone)
			rand > LAUNCH_LEFT_LOWER_BOUND) { // from the 4th bound to 2*pi (the other half of the right cone)
			a = rand;
			//	cout << ": accepted";
		}
	}

	return a;
}

Ball::Ball(float screenWidth, float screenHeight) {
	initX = screenWidth / 2.f;
	initY = screenHeight / 2.f;
	x = initX;
	y = initY;

	// set launch parameters
	const float screenDiagonalAngle = atan((float)screenHeight / (float)screenWidth);
	LAUNCH_LEFT_UPPER_BOUND = screenDiagonalAngle;
	LAUNCH_RIGHT_UPPER_BOUND = (float) M_PI - screenDiagonalAngle;
	LAUNCH_RIGHT_LOWER_BOUND = (float) M_PI + screenDiagonalAngle;
	LAUNCH_LEFT_LOWER_BOUND = 2.f * (float) M_PI - screenDiagonalAngle;

	// set up RNG
	std::random_device rd; // obtain a random number from hardware
	this->gen = std::mt19937(rd()); // seed the generator
	this->distr = std::uniform_real_distribution<>(0, 2.f * (float) M_PI); // define the range

	// create the render object
	this->renderObj = sf::CircleShape();
	this->renderObj.setRadius(RADIUS);
	this->renderObj.setFillColor(COLOR);
	updateRenderPos();

	angle = calcLaunchAngle();
}

void Ball::move() {
	x += cos(angle) * SPEED;
	y += sin(angle) * SPEED;
	updateRenderPos();
}

// inverts on y axis
void Ball::padCollision() {
	// varies depending on quadrant so we can ensure angle stays 0 <= angle < 2*pi
	if (angle <= (float)M_PI) {
		angle = (float)M_PI - angle;
	}
	else if (angle <= (float)M_PI + (float)M_PI_2) {
		angle = (2.f * (float)M_PI) - (angle - (float)M_PI);
	}
	else {
		angle = (3.f * (float)M_PI) - angle;
	}
	updateRenderPos();
}

// inverts on x axis
void Ball::wallCollision() {
	angle = (2.f * (float)M_PI) - angle;
	updateRenderPos();
}

void Ball::reset() {
	x = initX;
	y = initY;
	angle = calcLaunchAngle();
	updateRenderPos();
}