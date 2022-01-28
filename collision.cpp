#include "ball.hpp"
#include "pad.hpp"

#include "collision.hpp"

namespace collision {
	
	// strategy for having private memebers in a namespace
	namespace {

		typedef struct vector_t {
			float x, y;
		} vector;

		typedef struct hitbox_t {
			vector topLeft, topRight, bottomLeft, bottomRight;
		} hitbox;

		hitbox getBallHitbox(Ball* ball) {
			const float ballHitBoxSize = 2 * ball->RADIUS;
			return {
				{ ball->x,					ball->y					 },
				{ ball->x + ballHitBoxSize, ball->y					 },
				{ ball->x,					ball->y + ballHitBoxSize },
				{ ball->x + ballHitBoxSize, ball->y + ballHitBoxSize }
			};
		}

		bool checkImpl(Ball* ball, Pad* pad) {
			bool isCollision = false;

			const hitbox ballHitbox = getBallHitbox(ball);

			const hitbox padHitbox = {
				{ pad->x,				pad->y				 },
				{ pad->x + pad->SIZE_X, pad->y				 },
				{ pad->x,				pad->y + pad->SIZE_Y },
				{ pad->x + pad->SIZE_X, pad->y + pad->SIZE_Y }
			};

			if (
				ballHitbox.topRight.x > padHitbox.topLeft.x &&
				ballHitbox.topLeft.x < padHitbox.topRight.x &&
				ballHitbox.bottomLeft.y > padHitbox.topLeft.y &&
				ballHitbox.topLeft.y < padHitbox.bottomLeft.y
				) {
				isCollision = true;
			}

			return isCollision;
		}

		bool checkImpl(Ball* ball, float y) {
			bool isCollision = false;

			const hitbox ballHitbox = getBallHitbox(ball);

			if (
				(y == 0.f && ball->y <= 0.f) || // collision with the top wall
				(y != 0.f && ball->y >= y) // collision with the bottom wall
				) {
				isCollision = true;
			}

			return isCollision;
		}
	}

	bool check(Ball* ball, Pad* pad) {
		return checkImpl(ball, pad);
	}

	bool check(Ball* ball, float y) {
		return checkImpl(ball, y);
	}

}
