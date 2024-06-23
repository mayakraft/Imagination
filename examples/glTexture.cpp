#include <time.h>
#include "../include/engine.h"

int main() {
	int SCREEN = 640;
	int frame = 0;

	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "GL Texture Test",
		.width = SCREEN,
		.height = SCREEN,
		.useLegacy = true,
	};
	GameEngine engine = initGLEngine(params);

	GLfloat perspective[16];
	GLfloat lookat[16];
	GLuint wallTexture = loadGLTextureFromFile("examples/images/wall.png");
	GLuint bookTexture = loadGLAlphaTextureFromFile("examples/images/book.png");
	GLuint moonTexture = loadGLTextureFromFile("examples/images/moon.png");
	GLuint jupiterTexture = loadGLTextureFromFile("examples/images/jupiter.png");
	mesh_t plane, circle, moon, jupiter;

	makePerspectiveMatrix4(45, 1.0 / 1.0, 0.1f, 100.0f, perspective);
	makeQuadPlane(&plane, 2, 2);
	makeCircle(&circle);
	makeUVSphere(&moon, 1);
	makeCube(&jupiter);

	for (int v = 0; v < plane.numVertices; v++) {
		plane.vertices[v * 3 + 0] -= 1;
		plane.vertices[v * 3 + 1] -= 1;
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(perspective);

	// model view matrix
	glMatrixMode (GL_MODELVIEW);

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = true; }
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_f:
					// SDL_SetWindowFullscreen(engine.window, SDL_WINDOW_FULLSCREEN);
					break;
				}
			}
		}

		float time = frame / 60.0f;
		makeLookAtMatrix4(6 * cos(time), 6 * sin(time), 4, 0, 0, 0, 0, 0, 1, lookat);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		glLoadIdentity();
		glLoadMatrixf(lookat);

		glBindTexture(GL_TEXTURE_2D, wallTexture);
		drawMeshFaces(&plane);
		glBindTexture(GL_TEXTURE_2D, 0);

		glPushMatrix();
		glTranslatef(0, 2 * cos(time), 0.001);
		glBindTexture(GL_TEXTURE_2D, bookTexture);
		drawMeshFaces(&plane);
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, -2, 0);
		glBindTexture(GL_TEXTURE_2D, wallTexture);
		drawMeshFaces(&circle);
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(2, 0, 0);
		glBindTexture(GL_TEXTURE_2D, moonTexture);
		drawMeshStripArray(&moon);
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-2, 0, 0);
		glBindTexture(GL_TEXTURE_2D, jupiterTexture);
		drawMeshFaces(&jupiter);
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();

		SDL_GL_SwapWindow(engine.window);

		frame += 1;
	}

	dealloc(&engine);
	return 0;
}
