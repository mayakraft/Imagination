#include <cstdlib>
#include <random>
#include <stdlib.h>
#include <vector>
#include <ranges>
#include <iostream>

std::default_random_engine engine = std::default_random_engine {};

// low is inclusive, high is exclusive
int randInt(int low, int high) { return rand() % (high - low) + low; }

// direction encoding: 0, 1, 2, 3 relate to: top right bottom left (clockwise)
unsigned char oppositeDir(unsigned char direction) {
	return (direction + 2) % 4;
}

// get the numbers 0, 1, 2, 3 (all directions), in a scrambled order
std::vector<unsigned char> scrambleIntegers(int low, int high) {
	std::vector<unsigned char> ints = std::vector<unsigned char>();
	for (int i = low; i < high; i++) { ints.push_back(i); }
	std::shuffle(std::begin(ints), std::end(ints), engine);
	return ints;
}

std::vector<unsigned char> allDirections(int currentDirection) {
	std::vector<unsigned char> dirs = scrambleIntegers(0, 4);
	for (int i = 0; i < dirs.size(); i++) {
		if (dirs[0] == currentDirection) { return dirs; }
		int last = dirs[dirs.size() - 1];
		dirs.pop_back();
		dirs.insert(dirs.begin(), last);
	}
	return dirs;
}

// positive integers only.
// x:-1 y:-1 denotes an invalid node
struct node {
	int x;
	int y;
};

// top, right, bottom, left:
//   0 means wall
//   1 means open
// visited:
//   0 means unvisited
//   1 means visited
struct place {
	bool top;
	bool right;
	bool bottom;
	bool left;
	bool visited;
	unsigned char encode() {
		return top | (right << 1) | (bottom << 2) | (left << 3);
	}
	// this character represents the walkable path
	std::string getChar() {
		switch (encode()) {
			case 0: return " ";
			case 1: return "\u2575"; // t
			case 2: return "\u2576"; // r
			case 3: return "\u2514"; // t r
			case 4: return "\u2577"; // b
			case 5: return "\u2502"; // t b
			case 6: return "\u250c"; // r b
			case 7: return "\u251c"; // t r b
			case 8: return "\u2574"; // l
			case 9: return "\u2518"; // t l
			case 10: return "\u2500"; // r l
			case 11: return "\u2534"; // t r l
			case 12: return "\u2510"; // b l
			case 13: return "\u2524"; // t b l
			case 14: return "\u252c"; // r b l
			case 15: return "\u253c"; // t r b l
			default: return " ";
		}
	}
};

struct board {
	int size;
	place* places;

	static board init(int sidecount) {
		board b;
		b.size = sidecount;
		b.places = (place*)calloc(b.size * b.size, sizeof(place));
		return b;
	}

	void dealloc() { free(places); }

	int coord(int x, int y) { return y * size + x; }

	node stepNode(node n, unsigned char direction) {
		node step = node { .x = n.x, .y = n.y };
		if (direction == 0) step.y = n.y - 1;
		if (direction == 1) step.x = n.x + 1;
		if (direction == 2) step.y = n.y + 1;
		if (direction == 3) step.x = n.x - 1;
		return step;
	}

	bool isValid(node n) {
		return n.x >= 0 && n.y >= 0 && n.x < size && n.y < size;
	}

	bool isVisited(node n) {
		return places[coord(n.x, n.y)].visited;
	}

	void openWall(node n, unsigned char direction) {
		int i = coord(n.x, n.y);
		switch (direction) {
			case 0: places[i].top = 1; break;
			case 1: places[i].right = 1; break;
			case 2: places[i].bottom = 1; break;
			case 3: places[i].left = 1; break;
			default: break;
		}
	}

	// this prints the walkable path. the dual of the wall graph.
	std::string toString() {
		std::string str;
		for (int i = 0; i < size * size; i++) {
			str += places[i].getChar();
			if ((i + 1) % size == 0) str += "\n";
		}
		return str;
	}
};

// random starting place, the direction of the path tries to match
// the previous direction, with some tendency to change direction,
// even if it's not blocked.
void populate(board *maze, node current, int lastDirection) {
	maze->places[maze->coord(current.x, current.y)].visited = true;
	// at this current location, get a list of all possible directions
	// we will recurse into each direction. only at the moment of recursion
	// ensure that the path is clear (currently a wall and unvisited)
	std::vector<unsigned char> dirs = randInt(0, 2) == 0
		? scrambleIntegers(0, 4)
		: allDirections(lastDirection);

	for (int dir : dirs) {
		node next = maze->stepNode(current, dir);
		// the node is valid if it's inside the board and it's unvisited
		bool valid = maze->isValid(next) && !maze->isVisited(next);
		if (!valid) { continue; }
		maze->openWall(current, dir);
		maze->openWall(next, oppositeDir(dir));
		populate(maze, next, dir);
	}
}

struct maze : board {
	static maze init(int sidecount) {
		maze m;
		m.size = sidecount;
		m.places = (place*)calloc(m.size * m.size, sizeof(place));

		srand(time(NULL));
		engine.seed(time(NULL));
		node first = node {
			.x = randInt(0, m.size),
			.y = randInt(0, m.size),
		};
		populate(&m, first, randInt(0, 4));

		return m;
	}
};
