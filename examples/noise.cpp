// writing Simplex noise to a texture
#include "../include/engine.h"
#include "./misc/SimplexNoise.cpp"

void fillTexture(unsigned char* pixels, int width, int height, unsigned int frame) {
	float aberration = powf(4, SimplexNoise::noise(frame * 0.001 + 1)) * 0.0333;
	// aberration = powf(4, SimplexNoise::noise(frame * 0.001 + 1)) * 0.5;
	float scale = powf(4, SimplexNoise::noise(frame * 0.001)) * (0.5 / width);
	float baseRX = SimplexNoise::noise(frame * 0.003 + aberration * 0);
	float baseGX = SimplexNoise::noise(frame * 0.003 + aberration * 1);
	float baseBX = SimplexNoise::noise(frame * 0.003 + aberration * 2);
	float baseRY = SimplexNoise::noise(frame * 0.003 + 22 + aberration * 0);
	float baseGY = SimplexNoise::noise(frame * 0.003 + 22 + aberration * 1);
	float baseBY = SimplexNoise::noise(frame * 0.003 + 22 + aberration * 2);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			float red = SimplexNoise::noise(baseRX + i * scale, baseRY + j * scale);
			float green = SimplexNoise::noise(baseGX + i * scale, baseGY + j * scale);
			float blue = SimplexNoise::noise(baseBX + i * scale, baseBY + j * scale);
			int index = i * height + j;
			pixels[index * 3 + 0] = red * 127 + 127;
			pixels[index * 3 + 1] = green * 127 + 127;
			pixels[index * 3 + 2] = blue * 127 + 127;
		}
	}
}

int main() {
	int RES = 64;
	unsigned int frame = 0;
	bool flat = true;

	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "Simplex Noise",
		.width = 640,
		.height = 640,
		.useLegacy = true,
	};
	GameEngine engine = initGLEngine(params);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	unsigned char pixels[RES * RES * 3];
	GLuint texture = loadGLTexture(pixels, RES, RES, GL_RGB, GL_RGB, GL_NEAREST);

	mesh_t plane;
	makeRect(&plane, -1, -1, 2, 2);

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = true; }
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					// case SDLK_f:
					// SDL_SetWindowFullscreen(engine.window, SDL_WINDOW_FULLSCREEN);
					// break;
					default:
					flat = !flat;
					if (flat) {
						glMatrixMode(GL_PROJECTION);
						glLoadIdentity();
						glMatrixMode (GL_MODELVIEW);
						glLoadIdentity();
					} else {
						glMatrixMode(GL_PROJECTION);
						glLoadIdentity();
						GLfloat perspective[16];
						makePerspectiveMatrix4(45, 1.0 / 1.0, 0.1f, 100.0f, perspective);
						glLoadMatrixf(perspective);
						glMatrixMode (GL_MODELVIEW);
						glLoadIdentity();
						GLfloat lookat[16];
						makeLookAtMatrix4(0, -2, 3, 0, 0, 0, 0, 0, 1, lookat);
						glLoadMatrixf(lookat);
					}
					break;
				}
			}
		}

		fillTexture(pixels, RES, RES, frame);
		updateGLTexture(texture, pixels, RES, RES, GL_RGB, GL_NEAREST);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		glPushMatrix();
		if (!flat) {
			glRotatef(frame / 4.0, 0, 0, 1);
		}

		glBindTexture(GL_TEXTURE_2D, texture);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, plane.vertices);
		glTexCoordPointer(2, GL_FLOAT, 0, plane.texCoords);
		glDrawElements(GL_TRIANGLES, plane.numFaces * 3, GL_UNSIGNED_INT, plane.faces);
		glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glBindTexture(GL_TEXTURE_2D, 0);

		glPopMatrix(); // rotate

		SDL_GL_SwapWindow(engine.window);

		frame += 1;
	}

	dealloc(&engine);
	return 0;
}
