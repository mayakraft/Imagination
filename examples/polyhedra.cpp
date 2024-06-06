// this demonstrates rendering a triangle
// using the fixed-function pipeline
// and very old OpenGL syntax style.
#include <math.h>
#include "../src/engine.h"
#include "../src/primitives.h"
#include "../src/draw.h"
#include "SDL2/SDL_opengl.h"

static const GLfloat colors20[] = {
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.5f,
	0.0f, 0.5f, 0.5f,
	0.5f, 0.0f, 0.5f,
	0.5f, 0.5f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.5f,
	0.0f, 0.5f, 0.5f,
	0.5f, 0.0f, 0.5f,
	0.5f, 0.5f, 0.0f,
};

int main(int argc, char* args[]) {
	int SCREEN = 640;
	int frame = 0;

	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "Polyhedra",
		.width = SCREEN,
		.height = SCREEN,
		.disableShaders = true,
	};
	GameEngine engine = init3D(params);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 1.0, 0.1f, 100.0f);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, -3, -1, 0, 0, 0, 0, 0, 1);

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = true; }
		}

		glPushMatrix();
		glRotatef(frame, 0, 0, 1);

		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, colors20);

		drawPolyhedronFaces(&tetrahedron, 4);
		drawPolyhedronFaces(&tetrahedronDual, 4);

		glPushMatrix();
		float sPoly = ((cos(frame / 100.0) * 0.5) + 0.5) * 0.8 + 0.2;
		glScalef(0.2 + sPoly, 0.2 + sPoly, 0.2 + sPoly);
		drawPolyhedronFaces(&icosahedron, 20);
		drawPolyhedronFaces(&dodecahedron, 36);

		glDisableClientState(GL_COLOR_ARRAY);

		glColor4f(1.0, 1.0, 1.0, 1.0);
		drawPlatonicSolidLines(&icosahedron, 20);
		drawPlatonicSolidLines(&dodecahedron, 30);

		glPopMatrix();

		glColor4f(1.0, 1.0, 1.0, 1.0);
		drawPlatonicSolidLines(&tetrahedron, 6);
		drawPlatonicSolidLines(&tetrahedronDual, 6);

		glPopMatrix();

		SDL_GL_SwapWindow(engine.window);

		frame += 1;
	}

	return 0;
}
