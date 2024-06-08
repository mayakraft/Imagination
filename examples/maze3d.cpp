#include <math.h>
#include "../src/engine.h"
#include "../src/primitives.h"
#include "../src/drawgl.h"
#include "../src/math.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keyboard.h"
#include "SDL2/SDL_opengl.h"

#include "SDL2/SDL_pixels.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_surface.h"
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

// GLuint loadTexture(void* pixels, int width, int height){
// 	GLuint texture;
// 	glGenTextures(1, &texture);
// 	glBindTexture(GL_TEXTURE_2D, texture);
// 	// glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

// 	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
// 	// GL_RGBA
// 	glBindTexture(GL_TEXTURE_2D, 0);
// 	return texture;
// }

GLuint loadTexture(unsigned char *data, int width, int height){
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGRA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

int main(int argc, char **argv) {
	srand(time(NULL));
	int SCREEN = 640;
	int MAZE_SIZE = 8;
	int frame = 0;

	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "Maze 3D",
		.width = SCREEN,
		.height = SCREEN,
		.disableShaders = true,
	};
	GameEngine engine = init3D(params);

	maze m = maze::init(MAZE_SIZE);

	glEnable(GL_TEXTURE_2D);

	SDL_Surface* wall = IMG_Load("examples/images/wall.png");
	GLuint wallTexture = loadTexture((unsigned char*)wall->pixels, 32, 32);
	SDL_Surface* floor = IMG_Load("examples/images/floor.png");
	GLuint floorTexture = loadTexture((unsigned char*)floor->pixels, 32, 32);
	SDL_Surface* ceiling = IMG_Load("examples/images/ceiling.png");
	GLuint ceilingTexture = loadTexture((unsigned char*)ceiling->pixels, 32, 32);

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
			if (p.right) {
				auto rightWall = makeWall(x + 1, y, x + 1, y + 1);
				auto wallUV = makeWallTexCoords();
				verticesList.insert(verticesList.end(), rightWall.begin(), rightWall.end());
				texCoordsList.insert(texCoordsList.end(), wallUV.begin(), wallUV.end());
			}
			if (p.bottom) {
				auto bottomWall = makeWall(x, y + 1, x + 1, y + 1);
				auto wallUV = makeWallTexCoords();
				verticesList.insert(verticesList.end(), bottomWall.begin(), bottomWall.end());
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

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = true; }
			else if (e.type == SDL_KEYDOWN) {
				// printf( "%c (0x%04X)\n", (char)e.key.keysym.sym, e.key.keysym.sym );
				switch (e.key.keysym.sym) {
					case SDLK_f:
					// SDL_SetWindowFullscreen(engine.window, SDL_WINDOW_FULLSCREEN);
					break;
				}
			}
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		// model view matrix
		float pos[] = { (MAZE_SIZE / 2.0f) + 0.5f, frame * 0.01f, 0.5 };
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		GLfloat lookat[16];
		makeLookAtMatrix4(
			pos[0], pos[1], pos[2],
			pos[0], pos[1] + 1, pos[2],
			0, 0, 1,
			lookat);
		glLoadMatrixf(lookat);

		// glPushMatrix();
		// glRotatef(frame / 4.0, 0, 0, 1);

		// glPushMatrix();
		// glTranslatef(-MAZE_SIZE / 2.0, -MAZE_SIZE / 2.0, 0);

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
		glDrawArrays(GL_TRIANGLES, 0, verticesList.size() / 3);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindTexture(GL_TEXTURE_2D, 0);

		// glPopMatrix(); // translate
		// glPopMatrix(); // rotate

		SDL_GL_SwapWindow(engine.window);

		frame += 1;
	}

	// printf("%s", m.toString().c_str());

	SDL_FreeSurface(wall);
	m.dealloc();
	dealloc(&engine);
	return 0;
}
