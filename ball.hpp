#pragma once

#include <SFML/Graphics.hpp>
#include <random>

class Ball
{
	const float 
		INITIAL_SPEED = 0.3f,
		SPEED_STEP = 0.02f; // how much the speed increases at each step

	const sf::Color
		COLOR = sf::Color::White;

	float currentSpeed;

	//rng stuff
	std::mt19937 gen;
	std::uniform_real_distribution<> distr;

	void updateRenderPos();
	float calcLaunchAngle();
	void increaseSpeed();

public:
	const float RADIUS = 10.f;
	
	float x, y, initX, initY,
		angle; // should go from 0 to 2*PI -- how to enforce it?
	sf::CircleShape renderObj;

	/*
	*	There are two cones, one for each pad, and the direction in which the ball will launch
	*	will be inside either cone. These variables are the angles of the 4 bounds that describe
	*	the cones (2 for each), in radians. They're just public for debugging.
	*/
	float LAUNCH_LEFT_UPPER_BOUND,
		LAUNCH_RIGHT_UPPER_BOUND,
		LAUNCH_RIGHT_LOWER_BOUND,
		LAUNCH_LEFT_LOWER_BOUND;

	Ball(float screenWidth, float screenHeight);
	void padCollision();
	void wallCollision();
	void move();
	void reset();
};