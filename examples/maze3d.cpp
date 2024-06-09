#include <math.h>
#include "../src/engine.h"
#include "../src/math.h"
#include "maze.cpp"

float WALL_TEX_SCALE = 3.0f;
float FLOOR_TEX_SCALE = 3.0f;
float CEILING_TEX_SCALE = 3.0f;

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

// very strange: macos from makefile requires GL_BGRA, xcode requires GL_RGB
GLuint loadTexture(unsigned char *data, int width, int height) {
	unsigned short format = GL_BGRA;
	// unsigned short format = GL_RGB;
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

enum MoveType {
	walk,
	rotateLeft,
	rotateRight,
};

int main(int argc, char **argv) {
	srand((unsigned int)time(NULL));
	int SCREEN = 640;
	int MAZE_SIZE = 8;
	// int frame = 0;

	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "Maze 3D",
		.width = SCREEN,
		.height = SCREEN,
		.disableShaders = true,
	};
	GameEngine engine = init3D(params);

	maze m = maze::init(MAZE_SIZE);
	// open two places, the start and end
	m.places[0].left = 1;
	m.places[MAZE_SIZE * MAZE_SIZE - 1].right = 1;
	// printf("%s", m.toString().c_str());

	SDL_Surface* wall = IMG_Load("examples/images/wall.png");
	GLuint wallTexture = loadTexture((unsigned char*)wall->pixels, 32, 32);
	SDL_Surface* floor = IMG_Load("examples/images/floor.png");
	GLuint floorTexture = loadTexture((unsigned char*)floor->pixels, 32, 32);
	SDL_Surface* ceiling = IMG_Load("examples/images/ceiling.png");
	GLuint ceilingTexture = loadTexture((unsigned char*)ceiling->pixels, 32, 32);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat perspective[16];
	makePerspectiveMatrix4(45, 1.0 / 1.0, 0.1f, 100.0f, perspective);
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
			place p = m.places[m.coord(x, y)];
			if (!p.right) {
				auto rightWall = makeWall(x + 1, y, x + 1, y + 1);
				auto wallUV = makeWallTexCoords();
				verticesList.insert(verticesList.end(), rightWall.begin(), rightWall.end());
				texCoordsList.insert(texCoordsList.end(), wallUV.begin(), wallUV.end());
			}
			if (!p.bottom) {
				auto bottomWall = makeWall(x, y + 1, x + 1, y + 1);
				auto wallUV = makeWallTexCoords();
				verticesList.insert(verticesList.end(), bottomWall.begin(), bottomWall.end());
				texCoordsList.insert(texCoordsList.end(), wallUV.begin(), wallUV.end());
			}
			// special cases
			if (x == 0 && !p.left) {
				auto leftWall = makeWall(x, y, x, y + 1);
				auto wallUV = makeWallTexCoords();
				verticesList.insert(verticesList.end(), leftWall.begin(), leftWall.end());
				texCoordsList.insert(texCoordsList.end(), wallUV.begin(), wallUV.end());
			}
			if (y == 0 && !p.top) {
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
	float direction = 0;
	float camera[2] = { 0, 0 };
	bool arrowKeys[4] = { 0, 0, 0, 0 };

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = true; }
			else if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
					case SDLK_LEFT: arrowKeys[3] = false; break;
					case SDLK_RIGHT: arrowKeys[1] = false; break;
					case SDLK_UP: arrowKeys[0] = false; break;
					case SDLK_DOWN: arrowKeys[2] = false; break;
					default: break;
				}
			}
			else if (e.type == SDL_KEYDOWN) {
				// printf( "%c (0x%04X)\n", (char)e.key.keysym.sym, e.key.keysym.sym );
				switch (e.key.keysym.sym) {
					case SDLK_f:
					// SDL_SetWindowFullscreen(engine.window, SDL_WINDOW_FULLSCREEN);
					break;
					case SDLK_LEFT: arrowKeys[3] = true; break;
					case SDLK_RIGHT: arrowKeys[1] = true; break;
					case SDLK_UP: arrowKeys[0] = true; break;
					case SDLK_DOWN: arrowKeys[2] = true; break;
					default:
					break;
				}
			}
		}

		if (arrowKeys[0]) {
			camera[0] += 0.05f * cos(direction);
			camera[1] += 0.05f * sin(direction);
		}
		if (arrowKeys[2]) {
			camera[0] -= 0.05f * cos(direction);
			camera[1] -= 0.05f * sin(direction);
		}
		if (arrowKeys[1]) {
			direction -= 0.05;
		}
		if (arrowKeys[3]) {
			direction += 0.05;
		}

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
			camera[0] + 0.5f + cos(direction),
			camera[1] + 0.5f + sin(direction),
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

		// frame += 1;
	}

	SDL_FreeSurface(wall);
	m.dealloc();
	dealloc(&engine);
	return 0;
}
