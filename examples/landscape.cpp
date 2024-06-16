#include <time.h>
#include "../include/engine.h"
#include "./misc/SimplexNoise.cpp"
#include "./misc/colors.c"

struct vec3 {
	float x;
	float y;
	float z;
};

float altitudeAt(float x, float y) {
	// return SimplexNoise::noise(x * 0.05, y * 0.05);
	float noise = SimplexNoise::noise(x * 0.125, y * 0.125)
		// * SimplexNoise::noise(x * 0.05, y * 0.05)
		+ SimplexNoise::noise(14 + x * 0.05, 14 + y * 0.05)
		+ SimplexNoise::noise(33 + x * 0.02, 33 + y * 0.02) * 2;
	return noise * 0.8;
}

void colorAt(float alt, float* colors) {
	if (alt > 0.5) {
		colors[0] = 1.0;
		colors[1] = 1.0;
		colors[2] = 1.0;
		return;
	}
	if (alt < -0.5) {
		colors[0] = 0.1;
		colors[1] = 0.3;
		colors[2] = 1.0;
		return;
	}
	colors[0] = 0.3;
	colors[1] = 0.7;
	colors[2] = 0.1;
}

void fillVerticesAndColor(
	float* vertices,
	float* colors,
	int width,
	int height,
	float xPos,
	float yPos
) {
	float zScale = 1.0; // 2.0
	float colorScale = 0.05;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = x + y * width;
			float alt = altitudeAt(
				vertices[index * 3 + 0] + xPos,
				vertices[index * 3 + 1] + yPos);
			vertices[index * 3 + 2] = alt * zScale;
			// colorAt(alt, &colors[index * 3]);
			float red = SimplexNoise::noise(0 + x * colorScale + xPos * 0.05, 0 + y * colorScale + yPos * 0.05);
			float green = SimplexNoise::noise(10 + x * colorScale + xPos * 0.05, 10 + y * colorScale + yPos * 0.05);
			float blue = SimplexNoise::noise(17 + x * colorScale + xPos * 0.05, 17 + y * colorScale + yPos * 0.05);
			colors[index * 3 + 0] = red;
			colors[index * 3 + 1] = green;
			colors[index * 3 + 2] = blue;
		}
	}
}

void fillVertices(
	float* vertices,
	int width,
	int height,
	float xPos,
	float yPos
) {
	float zScale = 1.0; // 2.0
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = x + y * width;
			float alt = altitudeAt(
				vertices[index * 3 + 0] + xPos,
				vertices[index * 3 + 1] + yPos);
			vertices[index * 3 + 2] = alt * zScale;
		}
	}
}

int main() {
	srand(time(NULL));
	int WIDTH = 256;
	int HEIGHT = 256;
	uint frame = 0;
	int numTriangles = (WIDTH - 1) * (HEIGHT - 1) * 2;

	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "Landscape",
		.width = 640,
		.height = 640,
		.disableShaders = true,
	};
	GameEngine engine = init3D(params);

	vec3 pos = { .x = 0, .y = 0, .z = 0 };

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat perspective[16];
	makePerspectiveMatrix4(45, 1.0 / 1.0, 0.5f, 1000.0f, perspective);
	glLoadMatrixf(perspective);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	GLfloat lookat[16];
	makeLookAtMatrix4(0, 0, 5, 0, 4, 4, 0, 0, 1, lookat);
	// makeLookAtMatrix4(0, -20, 5, 0, 0, 0, 0, 0, 1, lookat);
	glLoadMatrixf(lookat);

	unsigned short indices[numTriangles * 3];
	float vertices[WIDTH * HEIGHT * 3];
	float colors[WIDTH * HEIGHT * 3];

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			int index = x + y * WIDTH;
			vertices[index * 3 + 0] = x - (WIDTH / 2);
			vertices[index * 3 + 1] = y - (HEIGHT / 2);
			vertices[index * 3 + 2] = 0;
		}
	}

	for (int i = 0; i < numTriangles * 3; i++) { indices[i] = 0; }
	for (int y = 0; y < HEIGHT - 1; y++) {
		for (int x = 0; x < WIDTH - 1; x++) {
			int index = x + y * (WIDTH - 1);
			int index2 = x + y * WIDTH;
			indices[index * 6 + 0] = index2;
			indices[index * 6 + 1] = index2 + WIDTH;
			indices[index * 6 + 2] = index2 + 1;
			indices[index * 6 + 3] = index2 + WIDTH;
			indices[index * 6 + 4] = index2 + WIDTH + 1;
			indices[index * 6 + 5] = index2 + 1;
		}
	}

	fillVerticesAndColor(vertices, colors, WIDTH, HEIGHT, pos.x, pos.y);

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = true; }
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_f:
					float aspect;
					setFullscreenGL(&engine, &aspect);

					// projection matrix
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					GLfloat perspective[16];
					makePerspectiveMatrix4(45, aspect, 0.5f, 1000.0f, perspective);
					glLoadMatrixf(perspective);
					glMatrixMode(GL_MODELVIEW);

					break;
				}
			}
		}

		fillVerticesAndColor(vertices, colors, WIDTH, HEIGHT, pos.x, pos.y);
		// fillVertices(vertices, WIDTH, HEIGHT, pos.x, pos.y);
		pos.x = sin(frame * 0.01) * 10;
		pos.y += 0.25;

		unsigned char red, green, blue;
		float hue = fmod(frame * 0.0005, 1.0);
		float lightness = (sin(-M_PI_2 + frame * 0.005) * 0.5 + 0.5) * 0.5;
		hslToRgb(hue, 1.0, lightness, &red, &green, &blue);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		// glClearColor(0.1f, 0.2f, 0.6f, 1.0f);
		glClearColor(red / 255., green / 255., blue / 255., 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		glPushMatrix();
		// glRotatef(frame / 4.0, 0, 0, 1);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glColorPointer(3, GL_FLOAT, 0, colors);
		glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_SHORT, indices);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glPopMatrix();

		SDL_GL_SwapWindow(engine.window);

		frame += 1;
	}

	dealloc(&engine);
	return 0;
}
