// this is old OpenGL 2.0 syntax style
#include <math.h>
#include "../include/engine.h"
#include "./misc/colors.c"

int main() {
	int SCREEN = 640;
	int frame = 0;

	InitParams params;
	params.flags = SDL_INIT_VIDEO;
	params.title = "Polyhedra";
	params.width = SCREEN;
	params.height = SCREEN;
	params.useLegacy = 1;
	GameEngine engine = initGLEngine(params);

	mesh_t tetrahedron, tetrahedronDual, icosahedron, dodecahedron;
	makeTetrahedron(&tetrahedron);
	makeTetrahedronDual(&tetrahedronDual);
	makeIcosahedron(&icosahedron);
	makeDodecahedron(&dodecahedron);

	// rainbow of colors
	GLfloat colors[20 * 3];

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat perspective[16];
	makePerspectiveMatrix4(45, 1.0 / 1.0, 0.1f, 100.0f, perspective);
	glLoadMatrixf(perspective);

	// model view matrix
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	GLfloat lookat[16];
	makeLookAtMatrix4(0, 1, -3, 0, 0, 0, 0, 1, 0, lookat);
	glLoadMatrixf(lookat);

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
			else if (e.type == SDL_KEYDOWN) {
				// printf( "%c (0x%04X)\n", (char)e.key.keysym.sym, e.key.keysym.sym );
				switch (e.key.keysym.sym) {
					case SDLK_f:
					// SDL_SetWindowFullscreen(engine.window, SDL_WINDOW_FULLSCREEN);
					break;
				}
			}
		}

		unsigned char r, g, b;
		for (int i = 0; i < 20; i += 1) {
			float hue = fmod(i / 20.0 + frame / 50.0, 1.0);
			hslToRgb(hue, 1.0, 0.5, &r, &g, &b);
			colors[i * 3 + 0] = r / 255.0;
			colors[i * 3 + 1] = g / 255.0;
			colors[i * 3 + 2] = b / 255.0;
		}

		glPushMatrix();
		glRotatef(frame, 0, 0, 1);

		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, colors);

		drawMeshFaces(&tetrahedron, 4);
		drawMeshFaces(&tetrahedronDual, 4);

		glPushMatrix();
		float sPoly = ((cos(frame / 100.0) * 0.5) + 0.5) * 0.8 + 0.2;
		glScalef(0.2 + sPoly, 0.2 + sPoly, 0.2 + sPoly);
		drawMeshFaces(&icosahedron, 20);
		drawMeshFaces(&dodecahedron, 36);

		glDisableClientState(GL_COLOR_ARRAY);

		glColor4f(1.0, 1.0, 1.0, 1.0);
		drawMeshLines(&icosahedron, 20);
		drawMeshLines(&dodecahedron, 30);

		glPopMatrix();

		glColor4f(1.0, 1.0, 1.0, 1.0);
		drawMeshLines(&tetrahedron, 6);
		drawMeshLines(&tetrahedronDual, 6);

		glPopMatrix();

		SDL_GL_SwapWindow(engine.window);

		frame += 1;
	}

	return 0;
}
