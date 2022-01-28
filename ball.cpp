#include <SFML/Graphics.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <random>

#include "ball.hpp"

class Pad;

#include "pad.hpp"

using namespace std;

void Ball::updateRenderPos() {
	this->renderObj.setPosition(sf::Vector2f(x, y));
}

float Ball::calcLaunchAngle() {
	float a = 0.f;

	// TODO do this without a loop
	while (a == 0) { // make sure float 0 == int 0
		float rand = (float) this->distr(this->gen); // random float from 0 to 2 * PI

		if (rand < LAUNCH_LEFT_UPPER_BOUND || // from 0 to the 1st bound (half the right cone)
			(rand > LAUNCH_RIGHT_UPPER_BOUND && rand <= LAUNCH_RIGHT_LOWER_BOUND) || // from the 2nd to the 3rd bound (the left cone)
			rand > LAUNCH_LEFT_LOWER_BOUND) { // from the 4th bound to 2*pi (the other half of the right cone)
			a = rand;
		}
	}

	return a;
}

void Ball::increaseSpeed() {
	currentSpeed += SPEED_STEP;
}

Ball::Ball(float screenWidth, float screenHeight) {
	initX = screenWidth / 2.f;
	initY = screenHeight / 2.f;

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

	reset();
}

void Ball::move() {
	x += cos(angle) * currentSpeed;
	y += sin(angle) * currentSpeed;
	updateRenderPos();
}

void Ball::padCollision(Pad* pad) {
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

	// sketch to a routine where the ball angle varies on where it connected with the pad
	// 
	//// to account for the ball, we're using a virtual pad size PAD + BALL
	//// we shouldnt need to calculate this values at every collision
	//const float padTop = pad->y - DIAMETER;
	//const float padBottom = pad->y + pad->SIZE_Y;
	//const float padSize = pad->SIZE_Y + DIAMETER;

	//// 0 to 1, top to bottom
	//const float normalized = (y - padTop) / padSize;

	//bool isPadA = (angle > M_PI_2) && (angle < (M_PI + M_PI_2));

	//// there must be a more elegant way to make this calculation
	//if (normalized <= 0.5f) { // upper half of the pad
	//	angle = (-1 * normalized * PAD_COLLISION_ANGLE) + PAD_COLLISION_ANGLE_2;
	//}
	//else { // lower half of the pad
	//	angle = (2.f * M_PI) - ((normalized - 0.5f) * PAD_COLLISION_ANGLE) + PAD_COLLISION_ANGLE_2;
	//}


	//if (!isPadA) { // invert on Y axis -- TEMPORARY!
	//	if (angle <= (float)M_PI) {
	//		angle = (float)M_PI - angle;
	//	}
	//	else if (angle <= (float)M_PI + (float)M_PI_2) {
	//		angle = (2.f * (float)M_PI) - (angle - (float)M_PI);
	//	}
	//	else {
	//		angle = (3.f * (float)M_PI) - angle;
	//	}
	//}

	//cout << "\nnormalized: " << normalized << " angle: " << angle;

	increaseSpeed();
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
	currentSpeed = INITIAL_SPEED;
	angle = calcLaunchAngle();
	updateRenderPos();
}
