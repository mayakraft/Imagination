// this demonstrates rendering a triangle
// using OpenGL using the the fixed-function pipeline
// (the very old way of writing OpenGL).
#include <math.h>
#include "../src/engine.h"
#include "SDL2/SDL_opengl.h"

int main(int argc, char* args[]) {
	int SCREEN = 640;
	int frame = 0;

	InitParams params = {
		.flags = SDL_INIT_VIDEO,
		.title = "OpenGL Fixed-Function Pipeline Triangle",
		.width = SCREEN,
		.height = SCREEN,
		.disableShaders = 1,
	};
	GameEngine engine = init3D(params);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, 1, -1, -10.0, 10.0);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
		}

		float a = frame / 100.0f;
		float r = 2.0 / 3.0;
		const float vertices[] = {
			(0.0f + cosf(a) * r),
			(0.0f + sinf(a) * r),
			(0.0f + cosf(a + 2 * M_PI / 3) * r),
			(0.0f + sinf(a + 2 * M_PI / 3) * r),
			(0.0f + cosf(a + 2 * M_PI / 3 * 2) * r),
			(0.0f + sinf(a + 2 * M_PI / 3 * 2) * r),
		};
		static const GLfloat colors[] = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
		};

		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertices);
		glColorPointer(3, GL_FLOAT, 0, colors);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		// alternatively, we can use glBegin glEnd like this
		// glBegin(GL_TRIANGLES);
		// glColor3f(1.0f, 0.0f, 0.0f);
		// glVertex2d(vertices[0], vertices[1]);
		// glColor3f(0.0f, 1.0f, 0.0f);
		// glVertex2d(vertices[2], vertices[3]);
		// glColor3f(0.0f, 0.0f, 1.0f);
		// glVertex2d(vertices[4], vertices[5]);
		// glEnd();

		SDL_GL_SwapWindow(engine.window);

		frame += 1;
	}

	return 0;
}
