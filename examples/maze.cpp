#include <cstdlib>
#include <random>
#include <stdlib.h>
#include <vector>
#include <ranges>
#include <iostream>

std::default_random_engine engine = std::default_random_engine {};

// low is inclusive, high is exclusive
int randInt(int low, int high) { return rand() % (high - low) + low; };

// direction encoding: 0, 1, 2, 3 relate to: top right bottom left (clockwise)
unsigned char oppositeDir(unsigned char direction) {
	return (direction + 2) % 4;
};

// get the numbers 0, 1, 2, 3 (all directions), in a scrambled order
std::vector<unsigned char> scrambleIntegers(int low, int high) {
	std::vector<unsigned char> ints = std::vector<unsigned char>();
	for (int i = low; i < high; i++) { ints.push_back(i); }
	std::shuffle(std::begin(ints), std::end(ints), engine);
	return ints;
};

std::vector<unsigned char> allDirections(int currentDirection) {
	std::vector<unsigned char> dirs = scrambleIntegers(0, 4);
	for (int i = 0; i < dirs.size(); i++) {
		if (dirs[0] == currentDirection) { return dirs; }
		int last = dirs[dirs.size() - 1];
		dirs.pop_back();
		dirs.insert(dirs.begin(), last);
	}
	return dirs;
};

struct coord { int x; int y; };

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

	board () {
		size = 8;
		places = (place*)calloc(size * size, sizeof(place));
	}

	board(int s) : size{s} {
		places = (place*)calloc(size * size, sizeof(place));
	}

	~board() {
		free(places);
	}

	place* getPlace(int x, int y) { return &places[y * size + x]; }

	coord stepNode(coord n, unsigned char direction) {
		coord step = coord { .x = n.x, .y = n.y };
		if (direction == 0) step.y = n.y - 1;
		if (direction == 1) step.x = n.x + 1;
		if (direction == 2) step.y = n.y + 1;
		if (direction == 3) step.x = n.x - 1;
		return step;
	};

	bool isValid(coord n) {
		return n.x >= 0 && n.y >= 0 && n.x < size && n.y < size;
	};

	bool isVisited(coord n) {
		return getPlace(n.x, n.y)->visited;
	};

	void openWall(coord n, unsigned char direction) {
		// switch statement is throwing off the linter/parser for some reason
		// switch (direction) {
		// 	case 0: getPlace(n.x, n.y)->top = 1; break;
		// 	case 1: getPlace(n.x, n.y)->right = 1; break;
		// 	case 2: getPlace(n.x, n.y)->bottom = 1; break;
		// 	case 3: getPlace(n.x, n.y)->left = 1; break;
		// 	default: break;
		// };
		if (direction == 0) { getPlace(n.x, n.y)->top = 1; }
		else if (direction == 1) { getPlace(n.x, n.y)->right = 1; }
		else if (direction == 2) { getPlace(n.x, n.y)->bottom = 1; }
		else if (direction == 3) { getPlace(n.x, n.y)->left = 1; }
	};

	bool isOpen(coord n, unsigned char direction) {
		if (!isValid(stepNode(n, direction))) { return false; }
		if (direction == 0) { return getPlace(n.x, n.y)->top; }
		else if (direction == 1) { return getPlace(n.x, n.y)->right; }
		else if (direction == 2) { return getPlace(n.x, n.y)->bottom; }
		else if (direction == 3) { return getPlace(n.x, n.y)->left; }
		return false;
	};

	// this prints the walkable path. the dual of the wall graph.
	std::string toString() {
		std::string str;
		for (int i = 0; i < size * size; i++) {
			str += places[i].getChar();
			if ((i + 1) % size == 0) str += "\n";
		}
		return str;
	};
};

// random starting place, the direction of the path tries to match
// the previous direction, with some tendency to change direction,
// even if it's not blocked.
void buildMazeRecurse(board *maze, coord current, int lastDirection) {
	maze->getPlace(current.x, current.y)->visited = true;
	// at this current location, get a list of all possible directions
	// we will recurse into each direction. only at the moment of recursion
	// ensure that the path is clear (currently a wall and unvisited)
	std::vector<unsigned char> dirs = randInt(0, 2) == 0
		? scrambleIntegers(0, 4)
		: allDirections(lastDirection);

	for (int dir : dirs) {
		coord next = maze->stepNode(current, dir);
		// the coord is valid if it's inside the board and it's unvisited
		bool valid = maze->isValid(next) && !maze->isVisited(next);
		if (!valid) { continue; }
		maze->openWall(current, dir);
		maze->openWall(next, oppositeDir(dir));
		buildMazeRecurse(maze, next, dir);
	}
};

struct maze : board {
	maze(int sidecount) : board(sidecount) {
		srand(time(NULL));
		engine.seed(time(NULL));
		coord first = coord {
			.x = randInt(0, size),
			.y = randInt(0, size),
		};
		buildMazeRecurse(this, first, randInt(0, 4));
	}
};

struct node {
	coord c;
	node *children[4];
};

void pathRecurse(board *maze, node *t) {
	coord current = coord { .x = t->c.x, .y = t->c.y };
	if (maze->getPlace(current.x, current.y)->visited) { return; }
	maze->getPlace(current.x, current.y)->visited = true;
	// iterate over all directions, recurse into each one if it is
	// a: valid (no wall between, not outside the maze)
	// a: unvisited at time of recurse
	unsigned char directions[] = { 0, 1, 2, 3 };
	// t.children = (node*)calloc(4, sizeof(node))
	for (int i = 0; i < 4; i++) {
		if (!maze->isOpen(current, directions[i])) { continue; }
		coord next = maze->stepNode(current, directions[i]);
		if (maze->getPlace(next.x, next.y)->visited) { continue; }
		node *nextTree = (node*)calloc(1, sizeof(node));
		nextTree->c = next;
		t->children[i] = nextTree;
		pathRecurse(maze, nextTree);
	}
};

node pathMaze(board *maze, coord start) {
	node root = node { .c = start };
	// clear all "visited" flags. mark the root visited
	int max = maze->size * maze->size;
	for (int i = 0; i < max; i++) { maze->places[i].visited = false; }
	pathRecurse(maze, &root);
	return root;
};

void linearizeRecurse(node *n, std::vector<coord>*linear) {
	linear->push_back(n->c);
	unsigned char directions[] = { 0, 1, 2, 3 };
	for (auto dir : directions) {
		if (n->children[dir] == NULL) { continue; }
		linearizeRecurse(n->children[dir], linear);
		linear->push_back(n->c);
	}
}

std::vector<coord> linearizeMaze(node *root) {
	std::vector<coord> linearization = std::vector<coord>();
	linearizeRecurse(root, &linearization);
	return linearization;
};
