#pragma once

#include "ball.hpp"
#include "pad.hpp"

namespace collision {

	bool check(Ball* ball, Pad* pad);
	// this is a tricky one: if y==0 we check for a collision with the top wall, otherwise it's against the bottom wall
	bool check(Ball* ball, float y);
}