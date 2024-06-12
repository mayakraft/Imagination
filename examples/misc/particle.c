#include <math.h>

struct coord {
	int x;
	int y;
};

struct particle {
	coord position;
	coord velocity;
	void update(int xMin, int yMin, int xMax, int yMax);
};

void particle::update(int xMin, int yMin, int xMax, int yMax) {
	position.x += velocity.x;
	position.y += velocity.y;
	if (position.x < xMin) {
		position.x = xMin;
		velocity.x = +abs(velocity.x);
	}
	if (position.y < yMin) {
		position.y = yMin;
		velocity.y = +abs(velocity.y);
	}
	if (position.x > xMax) {
		position.x = xMax;
		velocity.x = -abs(velocity.x);
	}
	if (position.y > yMax) {
		position.y = yMax;
		velocity.y = -abs(velocity.y);
	}
}
