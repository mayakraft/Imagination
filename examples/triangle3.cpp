// this demonstrates rendering a triangle
// using the programmable pipeline,
// modern idiomatic OpenGL.
#include <math.h>
#include "../src/engine.h"
#include "../src/shader.h"

ShaderProgram createProgram2(
	const char *vertexShaderSource,
	const char *fragmentShaderSource
) {
	GLuint gVBO = 0;
	GLuint gIBO = 0;
	// https://discourse.libsdl.org/t/lazy-foo-sdl-and-modern-opengl-tutorial-vao/23020
	GLuint vaoId = 0;
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);
	// Generate program
	GLuint gProgramID = glCreateProgram();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Set vertex source
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// Compile vertex source
	glCompileShader(vertexShader);
	// Check vertex shader for errors
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
	if (vShaderCompiled != GL_TRUE) {
		printShaderLog(vertexShader);
		throw std::runtime_error("Unable to compile vertex shader");
	}
	// Attach vertex shader to program
	glAttachShader(gProgramID, vertexShader);
	// Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Set fragment source
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	// Compile fragment source
	glCompileShader(fragmentShader);
	// Check fragment shader for errors
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE) {
		printf("Unable to compile fragment shader %d!\n", fragmentShader);
		printShaderLog(fragmentShader);
		throw std::runtime_error("Unable to compile fragment shader");
	}
	// Attach fragment shader to program
	glAttachShader(gProgramID, fragmentShader);
	// Link program
	glLinkProgram(gProgramID);
	// Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(gProgramID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE) {
		printProgramLog(gProgramID);
		throw std::runtime_error("Error linking program");
	}

	// vertex attribute location
	GLint gPositionAttribute = glGetAttribLocation(gProgramID, "pos");
	if (gPositionAttribute == -1) {
		throw std::runtime_error("invalid attrib location");
	}

	// Initialize clear color
	// glClearColor(0.1f, 0.3f, 0.6f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// VBO data
	float scale = 1.0f; // 0.85f;
	GLfloat vertexData[] = {
		-scale, -scale,
		scale, -scale,
		scale, scale,
		-scale, scale
	};
	// IBO data
	GLuint indexData[] = { 0, 1, 2, 3 };
	// Create VBO
	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
	// Create IBO
	glGenBuffers(1, &gIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);

	return ShaderProgram {
		.programID = gProgramID,
		.positionAttribute = gPositionAttribute,
		.vbo = gVBO,
		.ibo = gIBO,
	};
}

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
	glUseProgram(NULL);
}

int main(int argc, char* args[]) {
	int SCREEN = 640;
	// int frame = 0;

	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "OpenGL Programmable Pipeline Triangle",
		.width = SCREEN,
		.height = SCREEN,
	};

	GameEngine engine = init3D(params);

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = true; }
		}

		// float a = frame / 100.0f;
		// float r = (SCREEN / 3.0);
		// const float xy[] = {
		// 	(SCREEN / 2.0f + cosf(a) * r),
		// 	(SCREEN / 2.0f + sinf(a) * r),
		// 	(SCREEN / 2.0f + cosf(a + 2 * M_PI / 3) * r),
		// 	(SCREEN / 2.0f + sinf(a + 2 * M_PI / 3) * r),
		// 	(SCREEN / 2.0f + cosf(a + 2 * M_PI / 3 * 2) * r),
		// 	(SCREEN / 2.0f + sinf(a + 2 * M_PI / 3 * 2) * r),
		// };

		// frame += 1;
	}

	dealloc(&engine);
	return 0;
}
