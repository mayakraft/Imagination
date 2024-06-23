#include <math.h>
#include <time.h>
#include "./misc/maze.cpp"
#include "../include/engine.h"

float WALL_TEX_SCALE = 3.0f;
float FLOOR_TEX_SCALE = 1.0f;
float CEILING_TEX_SCALE = 3.0f;

float radialLerp(float a, float b, float t) {
	float eps = 0.01;
	float u = 1 - t;
	if (a < M_PI / 2.0 + eps && b > M_PI * 3.0 / 2.0 - eps) a += M_PI * 2;
	if (b < M_PI / 2.0 + eps && a > M_PI * 3.0 / 2.0 - eps) b += M_PI * 2;
	return a * u + b * t;
}

std::vector<float> makeWall(float x1, float y1, float x2, float y2) {
	return std::vector<float> {
		x1, y1, 0,
		x2, y2, 0,
		x2, y2, 1,
		x2, y2, 1,
		x1, y1, 1,
		x1, y1, 0,
	};
}

std::vector<float> makeWallTexCoords() {
	return std::vector<float> {
		0, 0,
		WALL_TEX_SCALE, 0,
		WALL_TEX_SCALE, WALL_TEX_SCALE,
		WALL_TEX_SCALE, WALL_TEX_SCALE,
		0, WALL_TEX_SCALE,
		0, 0,
	};
}

int main() {
	srand((unsigned int)time(NULL));
	int SCREEN = 640;
	int MAZE_SIZE = 8;
	// int frame = 0;

	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "Maze 3D",
		.width = SCREEN,
		.height = SCREEN,
		.useLegacy = true,
	};
	GameEngine engine = initGLEngine(params);

	// board m = buildMaze(MAZE_SIZE);
	maze m = maze(MAZE_SIZE);
	printf("%s", m.toString().c_str());
	// open two places, the start and end
	// m.places[0].left = 1;
	// m.places[MAZE_SIZE * MAZE_SIZE - 1].right = 1;

	coord start = { .x = 0, .y = 0 };
	node tree = pathMaze(&m, start);
	std::vector<coord> path = linearizeMaze(&tree);

	GLuint wallTexture = loadGLTextureFromFile("examples/images/wall.png");
	GLuint floorTexture = loadGLTextureFromFile("examples/images/floor.png");
	GLuint ceilingTexture = loadGLTextureFromFile("examples/images/ceiling.png");

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	float aspect = 1.0;
	// setFullscreenGL(&engine, &aspect);

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat perspective[16];
	makePerspectiveMatrix4(60, aspect, 0.1f, 100.0f, perspective);
	glLoadMatrixf(perspective);

	// model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLfloat lookat[16];
	makeLookAtMatrix4(0, -10, 10, 0, 0, 0, 0, 0, 1, lookat);
	glLoadMatrixf(lookat);

	std::vector<float> verticesList = std::vector<float>();
	std::vector<float> texCoordsList = std::vector<float>();

	for (int y = 0; y < m.size; y++) {
		for (int x = 0; x < m.size; x++) {
			place *p = m.getPlace(x, y);
			if (!p->right) {
				auto rightWall = makeWall(x + 1, y, x + 1, y + 1);
				auto wallUV = makeWallTexCoords();
				verticesList.insert(verticesList.end(), rightWall.begin(), rightWall.end());
				texCoordsList.insert(texCoordsList.end(), wallUV.begin(), wallUV.end());
			}
			if (!p->bottom) {
				auto bottomWall = makeWall(x, y + 1, x + 1, y + 1);
				auto wallUV = makeWallTexCoords();
				verticesList.insert(verticesList.end(), bottomWall.begin(), bottomWall.end());
				texCoordsList.insert(texCoordsList.end(), wallUV.begin(), wallUV.end());
			}
			// special cases
			if (x == 0 && !p->left) {
				auto leftWall = makeWall(x, y, x, y + 1);
				auto wallUV = makeWallTexCoords();
				verticesList.insert(verticesList.end(), leftWall.begin(), leftWall.end());
				texCoordsList.insert(texCoordsList.end(), wallUV.begin(), wallUV.end());
			}
			if (y == 0 && !p->top) {
				auto topWall = makeWall(x, y, x + 1, y);
				auto wallUV = makeWallTexCoords();
				verticesList.insert(verticesList.end(), topWall.begin(), topWall.end());
				texCoordsList.insert(texCoordsList.end(), wallUV.begin(), wallUV.end());
			}
		}
	}
	float *vertices = verticesList.data();
	float *texCoords = texCoordsList.data();

	float floorVertices[] = {
		0, 0, 0,
		(float)MAZE_SIZE, 0, 0,
		(float)MAZE_SIZE, (float)MAZE_SIZE, 0,
		(float)MAZE_SIZE, (float)MAZE_SIZE, 0,
		0, (float)MAZE_SIZE, 0,
		0, 0, 0,
	};
	float ceilingVertices[] = {
		0, 0, 1,
		(float)MAZE_SIZE, 0, 1,
		(float)MAZE_SIZE, (float)MAZE_SIZE, 1,
		(float)MAZE_SIZE, (float)MAZE_SIZE, 1,
		0, (float)MAZE_SIZE, 1,
		0, 0, 1,
	};
	float floorTexCoords[] = {
		0, 0,
		MAZE_SIZE * FLOOR_TEX_SCALE, 0,
		MAZE_SIZE * FLOOR_TEX_SCALE, MAZE_SIZE * FLOOR_TEX_SCALE,
		MAZE_SIZE * FLOOR_TEX_SCALE, MAZE_SIZE * FLOOR_TEX_SCALE,
		0, MAZE_SIZE * FLOOR_TEX_SCALE,
		0, 0,
	};
	float ceilingTexCoords[] = {
		0, 0,
		MAZE_SIZE * CEILING_TEX_SCALE, 0,
		MAZE_SIZE * CEILING_TEX_SCALE, MAZE_SIZE * CEILING_TEX_SCALE,
		MAZE_SIZE * CEILING_TEX_SCALE, MAZE_SIZE * CEILING_TEX_SCALE,
		0, MAZE_SIZE * CEILING_TEX_SCALE,
		0, 0,
	};

	// float tween = 0.0;
	// float direction = 0;
	float camera[2] = { 0, 0 };
	float lookDir = 0;

	uint32_t startTime = SDL_GetTicks();
	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = true; }
			else if (e.type == SDL_KEYDOWN) {
				// printf( "%c (0x%04X)\n", (char)e.key.keysym.sym, e.key.keysym.sym );
				switch (e.key.keysym.sym) {
					case SDLK_f:
					float aspect;
					setFullscreenGL(&engine, &aspect);

					// projection matrix
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					GLfloat perspective[16];
					makePerspectiveMatrix4(60, aspect, 0.1f, 100.0f, perspective);
					glLoadMatrixf(perspective);
					glMatrixMode(GL_MODELVIEW);

					break;
				}
			}
		}

		// const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
		// if (currentKeyStates[SDL_SCANCODE_UP]) {
		// 	camera[0] += 0.05f * cos(direction);
		// 	camera[1] += 0.05f * sin(direction);
		// }
		// if (currentKeyStates[SDL_SCANCODE_DOWN]) {
		// 	camera[0] -= 0.05f * cos(direction);
		// 	camera[1] -= 0.05f * sin(direction);
		// }
		// if (currentKeyStates[SDL_SCANCODE_LEFT]) {
		// 	direction += 0.05;
		// }
		// if (currentKeyStates[SDL_SCANCODE_RIGHT]) {
		// 	direction -= 0.05;
		// }

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		// model view matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		GLfloat lookat[16];
		float pos[] = { camera[0] + 0.5f, camera[1] + 0.5f, 0.5f };
		float forwardPoint[] = {
			camera[0] + 0.5f + cos(lookDir),
			camera[1] + 0.5f + sin(lookDir),
			0.5f,
		};

		makeLookAtMatrix4(
			pos[0], pos[1], 0.5,
			forwardPoint[0], forwardPoint[1], 0.5,
			0, 0, 1,
			lookat);
		glLoadMatrixf(lookat);

		// floor
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, floorVertices);
		glTexCoordPointer(2, GL_FLOAT, 0, floorTexCoords);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindTexture(GL_TEXTURE_2D, 0);

		// ceiling
		glBindTexture(GL_TEXTURE_2D, ceilingTexture);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, ceilingVertices);
		glTexCoordPointer(2, GL_FLOAT, 0, ceilingTexCoords);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindTexture(GL_TEXTURE_2D, 0);

		// walls
		glBindTexture(GL_TEXTURE_2D, wallTexture);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
		glDrawArrays(GL_TRIANGLES, 0, verticesList.size() / 3.0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindTexture(GL_TEXTURE_2D, 0);

		SDL_GL_SwapWindow(engine.window);

		uint32_t currTime = SDL_GetTicks();
		float elapsedTime = (currTime - startTime) / 1000.0;
		int seconds = floor(elapsedTime);
		float fractionalPart = elapsedTime - seconds;
		float u = 1 - fractionalPart;

		coord curr = path[seconds % path.size()];
		coord next = path[(seconds + 1) % path.size()];
		coord nextNext = path[(seconds + 2) % path.size()];
		coord dirCurr = { .x = next.x - curr.x, .y = next.y - curr.y };
		coord dirNext = { .x = nextNext.x - next.x, .y = nextNext.y - next.y };
		float currLookDir = atan2(dirCurr.y, dirCurr.x);
		float nextLookDir = atan2(dirNext.y, dirNext.x);

		while (currLookDir < 0) currLookDir += 2 * M_PI;
		while (nextLookDir < 0) nextLookDir += 2 * M_PI;

		// lerp position
		float lerpX = curr.x * u + next.x * fractionalPart;
		float lerpY = curr.y * u + next.y * fractionalPart;
		// lerp look direction
		lookDir = radialLerp(currLookDir, nextLookDir, fractionalPart);

		camera[0] = lerpX;
		camera[1] = lerpY;
		// frame += 1;
	}

	dealloc(&engine);
	return 0;
}
