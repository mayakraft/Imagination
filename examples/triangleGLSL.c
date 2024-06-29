// this demonstrates rendering a triangle
// using the programmable pipeline (shaders),
// modern idiomatic OpenGL.
#include <math.h>
#include "../include/engine.h"

int main() {
	int SCREEN = 640;
	int frame = 0;

	InitParams params = {
		.flags = SDL_INIT_VIDEO,
		.title = "OpenGL Programmable Pipeline Triangle",
		.width = SCREEN,
		.height = SCREEN,
	};

	GameEngine engine = initGLEngine(params);

	// vertices positions are filled in the vertex shader
	GLfloat vertexData[] = { 0, 0, 0, 0, 0, 0 };
	GLuint indexData[] = { 0, 1, 2 };

	char* vertex = readFile("./examples/shaders/triangle.vert", NULL);
	char* fragment = readFile("./examples/shaders/triangle.frag", NULL);
	GLuint program = createShaderProgram(vertex, fragment);
	free(vertex);
	free(fragment);

	GLuint vbo = makeArrayBuffer(vertexData, sizeof(GLfloat) * 6);
	GLuint ebo = makeElementBuffer(indexData, sizeof(GLuint) * 3);

	GLint timeUniform = getUniform(program, "u_time");

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
		}

		// begin rendering
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		// uniforms
		glUniform1f(timeUniform, frame * 0.01);

		// draw
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
		glUseProgram(0);
		SDL_GL_SwapWindow(engine.window);

		frame += 1;
	}

	deallocProgram(program);
	dealloc(&engine);
	return 0;
}
