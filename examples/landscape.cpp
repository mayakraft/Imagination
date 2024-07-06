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

	mesh_t plane;
	makeUVPlane(&plane, WIDTH - 1, HEIGHT - 1);

	short vertices[plane.numVertices * 3];
	for (unsigned int i = 0; i < plane.numVertices * 3; i++) {
		vertices[i] = plane.vertices[i];
	}
	for (unsigned int i = 0; i < plane.numVertices; i++) {
		vertices[i * 3 + 0] -= (WIDTH / 2);
		vertices[i * 3 + 1] -= (HEIGHT / 2);
	}

	char shaderPath[256] = "./examples/shaders";
	// char shaderPath[256] = getMacBundleResourcesPath();
	std::string vertexPath = std::string(shaderPath) + "/landscape.vert";
	std::string fragmentPath = std::string(shaderPath) + "/landscape.frag";
	char* vertex = readFile(vertexPath.c_str(), NULL);
	char* fragment = readFile(fragmentPath.c_str(), NULL);

	GLuint program = createShaderProgram(vertex, fragment);
	GLuint vbo = makeArrayBuffer(vertices, plane.numVertices * 3 * sizeof(GLshort));
	GLuint ebo = makeElementBuffer(plane.faces, plane.numFaces * 3 * sizeof(GLuint));
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
