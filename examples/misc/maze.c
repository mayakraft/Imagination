#include <stdlib.h>
#include <string.h>
// #include <vector.h>
#include <string.h>
// #include <random>

// direction encoding: 0, 1, 2, 3 relate to: top right bottom left (clockwise)

// low is inclusive, high is exclusive
int randInt(int low, int high) { return rand() % (high - low) + low; };

// std::vector<unsigned char> scrambleIntegers(int low, int high) {
// 	// std::qsort();
// 	std::vector<unsigned char> ints = std::vector<unsigned char>();
// 	for (int i = low; i < high; i++) { ints.push_back(i); }
// 	std::random_device rd;
// 	std::mt19937 g(rd());
// 	std::shuffle(ints.begin(), ints.end(), g);
// 	return ints;
// }
void scrambleIntegers(int low, int high, unsigned char* result) {
	// std::qsort();
	unsigned char ints[high - low];
	for (int i = 0; i < high - low; i++) { ints[i] = i + low; }
	result = (unsigned char*)malloc(sizeof(unsigned char) * (high - low));
	memcpy(result, ints, sizeof(unsigned char) * (high - low));
	// shuffle
}

// this returns a scrambled list of directions, where the first entry
// is the current direction, the next three are the remaining three directions.
// std::vector<unsigned char> allDirections(int currentDirection) {
// 	std::vector<unsigned char> dirs = scrambleIntegers(0, 4);
// 	for (int i = 0; i < dirs.size(); i++) {
// 		if (dirs[0] == currentDirection) { return dirs; }
// 		int last = dirs[dirs.size() - 1];
// 		dirs.pop_back();
// 		dirs.insert(dirs.begin(), last);
// 	}
// 	return dirs;
// };

void allDirections(int currentDirection, unsigned char* result) {
	unsigned char* dirs;
	scrambleIntegers(0, 4, dirs);
	for (int i = 0; i < 4; i++) {
		if (dirs[0] == currentDirection) {
			result = (unsigned char*)malloc(sizeof(unsigned char) * 4);
			memcpy(result, dirs, sizeof(unsigned char) * 4);
		}
		// int last = dirs[dirs.size() - 1];
		// dirs.pop_back();
		// dirs.insert(dirs.begin(), last);
	}
	result = (unsigned char*)malloc(sizeof(unsigned char) * 4);
	memcpy(result, dirs, sizeof(unsigned char) * 4);
};

typedef struct coord {
	int x;
	int y;
} coord;

// top, right, bottom, left:  0=wall  1=open
// visited:  0=unvisited  1=visited
typedef struct place {
	unsigned char top;
	unsigned char right;
	unsigned char bottom;
	unsigned char left;
	unsigned char visited;
	// unsigned char encode();
	// std::string getChar();
} place;

unsigned char placeEncode(place *p) {
	return p->top | (p->right << 1) | (p->bottom << 2) | (p->left << 3);
}

// this character represents the walkable path
void placeGetChar(place *p, char** result) {
	// unsigned short place_lookup_table[16] = {
	// 	0x0020, 0x2575, 0x2576, 0x2514, 0x2577, 0x2502, 0x250c, 0x251c,
	// 	0x2574, 0x2518, 0x2500, 0x2534, 0x2510, 0x2524, 0x252c, 0x253c,
	// };
	switch (placeEncode(p)) {
		case 0: *result = " "; return;
		case 1: *result = "\u2575"; return; // t
		case 2: *result = "\u2576"; return; // r
		case 3: *result = "\u2514"; return; // t r
		case 4: *result = "\u2577"; return; // b
		case 5: *result = "\u2502"; return; // t b
		case 6: *result = "\u250c"; return; // r b
		case 7: *result = "\u251c"; return; // t r b
		case 8: *result = "\u2574"; return; // l
		case 9: *result = "\u2518"; return; // t l
		case 10: *result = "\u2500"; return; // r l
		case 11: *result = "\u2534"; return; // t r l
		case 12: *result = "\u2510"; return; // b l
		case 13: *result = "\u2524"; return; // t b l
		case 14: *result = "\u252c"; return; // r b l
		case 15: *result = "\u253c"; return; // t r b l
		default: *result = " "; return;
	}
}

typedef struct board {
	int size;
	place* places;
} board;

board makeBoard () {
	board b;
	b.size = 8;
	b.places = (place*)calloc(b.size * b.size, sizeof(place));
	return b;
}

board makeBoard(int s) {
	board b;
	b.size = s;
	b.places = (place*)calloc(b.size * b.size, sizeof(place));
	return b;
}

void deallocBoard(board* b) {
	free(b->places);
}

place* boardGetPlace(board* b, int x, int y) {
	return &b->places[y * b->size + x];
}

coord stepNode(coord n, unsigned char direction) {
	coord step = { n.x, n.y };
	if (direction == 0) step.y = n.y - 1;
	if (direction == 1) step.x = n.x + 1;
	if (direction == 2) step.y = n.y + 1;
	if (direction == 3) step.x = n.x - 1;
	return step;
};

unsigned char boardIsValid(board* b, coord n) {
	return n.x >= 0 && n.y >= 0 && n.x < b->size && n.y < b->size;
};

unsigned char boardIsVisited(board* b, coord n) {
	return boardGetPlace(b, n.x, n.y)->visited;
};

void boardOpenWall(board* b, coord n, unsigned char direction) {
	// switch statement is throwing off the linter/parser for some reason
	// switch (direction) {
	// 	case 0: getPlace(n.x, n.y)->top = 1; break;
	// 	case 1: getPlace(n.x, n.y)->right = 1; break;
	// 	case 2: getPlace(n.x, n.y)->bottom = 1; break;
	// 	case 3: getPlace(n.x, n.y)->left = 1; break;
	// 	default: break;
	// };
	if (direction == 0) { boardGetPlace(b, n.x, n.y)->top = 1; }
	else if (direction == 1) { boardGetPlace(b, n.x, n.y)->right = 1; }
	else if (direction == 2) { boardGetPlace(b, n.x, n.y)->bottom = 1; }
	else if (direction == 3) { boardGetPlace(b, n.x, n.y)->left = 1; }
};

unsigned char boardIsOpen(board* b, coord n, unsigned char direction) {
	if (!boardIsValid(b, stepNode(n, direction))) { return 0; }
	if (direction == 0) { return boardGetPlace(b, n.x, n.y)->top; }
	else if (direction == 1) { return boardGetPlace(b, n.x, n.y)->right; }
	else if (direction == 2) { return boardGetPlace(b, n.x, n.y)->bottom; }
	else if (direction == 3) { return boardGetPlace(b, n.x, n.y)->left; }
	return 1;
};

// this prints the walkable path. the dual of the wall graph.
void boardToString(board* b, char* str) {
	int num = 0;
	str = (char*)malloc(sizeof(char) * 4096);
	for (int i = 0; i < b->size * b->size; i++) {
		// placeGetChar(b->places[i], &str[num]);
		// placeGetChar(&(b->places[i]), &str[num]);
		num++;
		if ((i + 1) % b->size == 0) { str[num] = '\n'; }
		num++;
	}
};

// random starting place, the direction of the path tries to match
// the previous direction, with some tendency to change direction,
// even if it's not blocked.
void buildMazeRecurse(board *maze, coord current, int lastDirection) {
	boardGetPlace(maze, current.x, current.y)->visited = true;
	// at this current location, get a list of all possible directions
	// we will recurse into each direction. only at the moment of recursion
	// ensure that the path is clear (currently a wall and unvisited)
	std::vector<unsigned char> dirs = randInt(0, 2) == 0
		? scrambleIntegers(0, 4)
		: allDirections(lastDirection);

	for (int i = 0; i < 4; i++) {
		int dir = dirs[i];
		coord next = maze->stepNode(current, dir);
		// the coord is valid if it's inside the board and it's unvisited
		bool valid = maze->isValid(next) && !maze->isVisited(next);
		if (!valid) { continue; }
		maze->openWall(current, dir);
		maze->openWall(next, (dir + 2) % 4); // opposite direction
		buildMazeRecurse(maze, next, dir);
	}
};

struct maze : board {
	maze(int sidecount) : board(sidecount) {
		srand(time(NULL));
		coord first = {
			randInt(0, size),
			randInt(0, size),
		};
		buildMazeRecurse(this, first, randInt(0, 4));
	}
};

struct node {
	coord c;
	node *children[4];
};

void pathRecurse(board *maze, node *t) {
	coord current = { t->c.x, t->c.y };
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
	node root = { .c = start };
	// clear all "visited" flags. mark the root visited
	int max = maze->size * maze->size;
	for (int i = 0; i < max; i++) { maze->places[i].visited = false; }
	pathRecurse(maze, &root);
	return root;
};

void linearizeRecurse(node *n, std::vector<coord>*linear) {
	linear->push_back(n->c);
	for (int dir = 0; dir < 4; dir++) {
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
