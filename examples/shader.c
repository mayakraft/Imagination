// this demonstrates a simple full-screen
// animated fragment shader
#include <stdio.h>
#include "../include/engine.h"

void render(ShaderProgram* program, float time, float screenW, float screenH) {
	// Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	// Bind program
	glUseProgram(program->programID);
	// Enable vertex position
	glEnableVertexAttribArray(program->positionAttribute);
	// Set vertex data
	glBindBuffer(GL_ARRAY_BUFFER, program->vbo);
	// printf("gVertexPos2DLocation %d\n", gVertexPos2DLocation);
	glVertexAttribPointer(
		program->positionAttribute,
		2,
		GL_FLOAT,
		GL_FALSE,
		2 * sizeof(GLfloat),
		NULL);
	// uniforms
	GLint timeLocation = glGetUniformLocation(program->programID, "u_time");
	if (timeLocation >= 0) {
		glUniform1f(timeLocation, time);
	}
	GLint resLocation = glGetUniformLocation(program->programID, "u_resolution");
	if (resLocation >= 0) {
		glUniform2f(resLocation, screenW, screenH);
	}
	// Set index data and render
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, program->ibo);
	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);
	// Disable vertex position
	glDisableVertexAttribArray(program->positionAttribute);
	// Unbind program
	glUseProgram(0);
}

int main(void) {
	int SCREEN = 640;
	int frame = 0;

	InitParams params;
	params.flags = SDL_INIT_VIDEO;
	params.title = "Game Engine";
	params.width = SCREEN;
	params.height = SCREEN;
	GameEngine engine = init3D(params);

	glDebugInfo();

	char* vertex = readFile("./examples/shaders/simple.vert");
	char* fragment = readFile("./examples/shaders/kaleidoscope.frag");
	ShaderProgram program = createProgram(vertex, fragment);
	free(vertex);
	free(fragment);

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
		}

		// Initialize clear color
		glClearColor(0.1f, 0.3f, 0.6f, 1.0f);

		float scale = engine.pixelScale;
		render(&program, frame * 0.01, SCREEN * scale, SCREEN * scale);
		SDL_GL_SwapWindow(engine.window);

		frame++;
	}

	dealloc(&engine);
	return 0;
}
