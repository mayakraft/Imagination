// infinite scrolling simplex noise landscape,
// vertices positions calculated on the GPU

// #ifdef _WIN32
// #define SDL_MAIN_HANDLED
// #endif

#include <string>
#include <time.h>
#include "../include/engine.h"

int main() {
	srand(time(NULL));
	unsigned int frame = 0;
	const int WIDTH = 512;
	const int HEIGHT = 512;
	const int numTriangles = (WIDTH - 1) * (HEIGHT - 1) * 2;
	GLfloat projection[16];
	GLfloat modelView[16];

	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "Landscape",
		.width = 640,
		.height = 640,
	};
	GameEngine engine = initGLEngine(params);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glDebugInfo();

	float offset[2] = {
		(float)(rand() % 1000),
		(float)(rand() % 1000),
	};

	unsigned int indices[numTriangles * 3];
	short vertices[WIDTH * HEIGHT * 3];

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

	char shaderPath[256] = "./examples/shaders";
	// char shaderPath[256] = getMacBundleResourcesPath();
	std::string vertexPath = std::string(shaderPath) + "/landscape.vert";
	std::string fragmentPath = std::string(shaderPath) + "/landscape.frag";
	char* vertex = readFile(vertexPath.c_str(), NULL);
	char* fragment = readFile(fragmentPath.c_str(), NULL);

	GLuint program = createShaderProgram(vertex, fragment);
	GLuint vbo = makeArrayBuffer(vertices, 3 * WIDTH * HEIGHT * sizeof(GLshort));
	GLuint ebo = makeElementBuffer(indices, numTriangles * 3 * sizeof(GLuint));
	GLint vertexAttrib = getAttrib(program, "position");
	GLint timeLocation = getUniform(program, "u_time");
	GLint offsetLocation = getUniform(program, "u_offset");
	GLint projectionLocation = getUniform(program, "u_projection");
	GLint modelViewLocation = getUniform(program, "u_modelView");

	// makePerspectiveMatrix4(45, 1.0 / 1.0, 1.0f, 5000.0f, projection);
	makeOrthographicMatrix4(380, 380, -380, -380, 1, 2048.0, projection);
	makeLookAtMatrix4(480, 480, 600, 0, 0, 0, 0, 0, 1, modelView);

	// fill the VAO
	GLuint vao = beginVAO();
	glEnableVertexAttribArray(vertexAttrib);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(vertexAttrib, 3, GL_SHORT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	endVAO();

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
					// makePerspectiveMatrix4(45, aspect, 1.0f, 5000.0f, projection);
					makeOrthographicMatrix4(320, 320 * aspect, -320, -320 * aspect, 1, 2048.0, projection);
					break;
				}
			}
		}

		float time = frame * (1.0 / 60.0);
		offset[0] += 1;
		offset[1] -= 1;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glUseProgram(program);

		// uniforms
		glUniform1f(timeLocation, time);
		glUniform2f(offsetLocation, offset[0], offset[1]);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projection);
		glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView);

		// draw
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_INT, NULL);
		glBindVertexArray(0);

		SDL_GL_SwapWindow(engine.window);

		frame += 1;
	}

	deallocProgram(program);
	dealloc(&engine);
	return 0;
}
