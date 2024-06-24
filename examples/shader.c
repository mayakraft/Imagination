// this demonstrates a simple full-screen
// animated fragment shader
#include <stdio.h>
#include "../include/engine.h"

int main(void) {
	int SCREEN = 640;
	int frame = 0;

	InitParams params;
	params.flags = SDL_INIT_VIDEO;
	params.title = "Game Engine";
	params.width = SCREEN;
	params.height = SCREEN;
	GameEngine engine = initGLEngine(params);

	glDebugInfo();

	char* vertex = readFile("./examples/shaders/simple.vert", NULL);
	char* fragment = readFile("./examples/shaders/kaleidoscope.frag", NULL);
	ShaderProgram program = createProgram(vertex, fragment);
	free(vertex);
	free(fragment);

	GLfloat vertexData[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f,
		-1.0f, 1.0f
	};
	GLuint indexData[] = { 0, 1, 2, 3 };

	program.vbo = generateVertexBuffer(vertexData, sizeof(GLfloat) * 4 * 2);
	program.ebo = generateElementBuffer(indexData, sizeof(GLuint) * 4);
	GLint vertexAttrib = getAttrib(&program, "position");
	GLint timeUniform = getUniform(&program, "u_time");
	GLint resUniform = getUniform(&program, "u_resolution");

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
		}

		// Initialize clear color
		glClearColor(0.1f, 0.3f, 0.6f, 1.0f);

		float scale = engine.pixelScale;
		float time = frame * 0.01;

		// begin rendering
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program.programID);
		glBindBuffer(GL_ARRAY_BUFFER, program.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, program.ebo);

		// Enable vertex position
		glEnableVertexAttribArray(vertexAttrib);
		glVertexAttribPointer(vertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		// uniforms
		glUniform1f(timeUniform, time);
		glUniform2f(resUniform, SCREEN * scale, SCREEN * scale);

		// draw
		glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);

		// finish rendering
		glDisableVertexAttribArray(vertexAttrib);
		glUseProgram(0);

		SDL_GL_SwapWindow(engine.window);

		frame++;
	}

	deallocProgram(&program);
	dealloc(&engine);
	return 0;
}
