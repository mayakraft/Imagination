#include <string>
#include <time.h>
#include "../include/engine.h"
#include "./misc/SimplexNoise.cpp"
#include "./misc/colors.c"

struct vec3 {
	float x;
	float y;
	float z;
};

float altitudeAt(float x, float y, float seed) {
	// return SimplexNoise::noise(x * 0.05, y * 0.05);
	float noise = SimplexNoise::noise(seed + x * 0.02, seed + y * 0.02) * 1.5
		// * SimplexNoise::noise(seed + x * 0.05, seed + y * 0.05)
		+ SimplexNoise::noise(seed + 14 + x * 0.08, seed + 14 + y * 0.08)
		+ SimplexNoise::noise(seed + 33 + x * 0.15, seed + 33 + y * 0.15) * 0.5
		+ SimplexNoise::noise(seed + 33 + x * 0.4, seed + 33 + y * 0.4) * 0.3
		+ SimplexNoise::noise(seed + 41 + x * 0.5, seed + 41 + y * 0.5) * 0.15;
	return noise * 0.6;
}

void fillVertices(float* vertices, int width, int height, float xPos, float yPos, float seed) {
	float zScale = 1.0; // 2.0
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = x + y * width;
			float alt = altitudeAt(
				vertices[index * 3 + 0] + xPos,
				vertices[index * 3 + 1] + yPos,
				seed);
			vertices[index * 3 + 2] = alt * zScale;
		}
	}
}

void render(
	ShaderProgram* program,
	int numVertices, // this will get * 3
	int numIndices, // this will get no multiplier
	float* vertexBuffer,
	GLint vertexAttrib,
	float* projection,
	float* modelView,
	float time,
	float screenW,
	float screenH) {

	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program->programID);
	glEnableVertexAttribArray(vertexAttrib);

	// two ways of updating vertex data:
	// 1:
	// glBindBuffer(GL_ARRAY_BUFFER, program->vbo);
	// glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * numVertices * sizeof(GLfloat), vertexBuffer);
	// 2:
	glBindBuffer(GL_ARRAY_BUFFER, program->vbo);
	void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, vertexBuffer, 3 * numVertices * sizeof(GLfloat));
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribPointer(
		vertexAttrib,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		// 3 * sizeof(GLfloat),
		NULL);
	// uniforms
	GLint timeLocation = glGetUniformLocation(program->programID, "u_time");
	if (timeLocation >= 0) {
		glUniform1f(timeLocation, time);
	}
	GLint projectionLocation = glGetUniformLocation(program->programID, "u_projection");
	if (projectionLocation >= 0) {
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projection);
	}
	GLint modelViewLocation = glGetUniformLocation(program->programID, "u_modelView");
	if (modelViewLocation >= 0) {
		glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView);
	}

	// GLint resLocation = glGetUniformLocation(program->programID, "u_resolution");
	// if (resLocation >= 0) {
	// 	glUniform2f(resLocation, screenW, screenH);
	// }
	// Set index data and render
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, program->ibo);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, NULL);
	glDisableVertexAttribArray(vertexAttrib);
	glUseProgram(0);
}

int main() {
	srand(time(NULL));
	int WIDTH = 128;
	int HEIGHT = 128;
	uint frame = 0;
	int numTriangles = (WIDTH - 1) * (HEIGHT - 1) * 2;
	GLfloat projection[16];
	GLfloat modelView[16];
	float seed = rand() % 1000 * 0.01;

	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "Landscape",
		.width = 640,
		.height = 640,
	};
	GameEngine engine = init3D(params);

	glDebugInfo();

	char* vertex = readFile("./examples/shaders/landscape.vert");
	char* fragment = readFile("./examples/shaders/landscape.frag");
	// std::string vertex = "#version 140\nuniform mat4 u_projection;\nuniform mat4 u_modelView;\nin vec3 pos;\nout vec3 v_position;\nvoid main() {\nv_position = pos;\ngl_Position = u_projection * u_modelView * vec4(pos, 1.0);\n}";
	// std::string fragment = "#version 140\nuniform float u_time;\nin vec3 v_position;\nout vec4 outColor;\nvoid main() {\n	float altitude = v_position.z * 0.5 + 0.5;\n	vec3 c_grass = vec3(0.3, 0.7, 0.1) * altitude;\n	vec3 c_snow = vec3(1.0, 1.0, 1.0);\n	vec3 c_water = vec3(0.1, 0.3, 1.0) * (altitude + 0.3);\n	vec3 water = c_water * (1 - smoothstep(0.1, 0.3, altitude));\n	vec3 grass = c_grass * smoothstep(0.1, 0.3, altitude);\n	vec3 snow = c_snow * smoothstep(0.8, 0.9, altitude);\n	vec3 sum = water + grass + snow;\n	outColor = vec4(clamp(sum.x, 0.0, 1.0), clamp(sum.y, 0.0, 1.0), clamp(sum.z, 0.0, 1.0), 1.0);\n}\n";

	makePerspectiveMatrix4(45, 1.0 / 1.0, 0.5f, 1000.0f, projection);
	makeLookAtMatrix4(0, 0, 4, 0, 3, 3, 0, 0, 1, modelView);

	vec3 pos = { .x = 0, .y = 0, .z = 0 };

	unsigned short indices[numTriangles * 3];
	float vertices[WIDTH * HEIGHT * 3];

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

	fillVertices(vertices, WIDTH, HEIGHT, pos.x, pos.y, seed);

	// ShaderProgram shader = createShaderProgram(
	// 	vertex,
	// 	fragment,
	// 	// vertex.c_str(),
	// 	// fragment.c_str(),
	// 	vertices,
	// 	WIDTH * HEIGHT,
	// 	indices,
	// 	numTriangles * 3);

	ShaderProgram shader = createProgram(vertex, fragment);
	// vertex.c_str(),
	// fragment.c_str());

	GLint vertexAttrib = getAttrib(&shader, "position");

	// GLint getUniform(ShaderProgram *program, const char* name);

	// we need GL_DYNAMIC_DRAW here
	// generateVertexBuffer(&shader, vertices, 3 * WIDTH * HEIGHT * sizeof(GLfloat));
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * WIDTH * HEIGHT * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
	shader.vbo = vbo;

	// generateElementBuffer(&shader, indices, numTriangles * 3);
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numTriangles * 3 * sizeof(GLushort), indices, GL_STATIC_DRAW);
	shader.ibo = ibo;

	printf("shader.vbo %d\n", shader.vbo);
	printf("shader.ibo %d\n", shader.ibo);
	printf("vertexAttrib %d\n", vertexAttrib);

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
					makePerspectiveMatrix4(45, aspect, 0.5f, 1000.0f, projection);
					break;
				}
			}
		}

		float speed = 0.25;
		fillVertices(vertices, WIDTH, HEIGHT, pos.x, pos.y, seed);
		pos.x = sin(frame * 0.01) * 10 * speed;
		pos.y += 0.25 * speed;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearColor(0.05f, 0.15f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		// float scale = engine.pixelScale;
		// render(&shader, projection, modelView, frame * 0.01, engine.width * scale, engine.height * scale);
		render(
			&shader,
			WIDTH * HEIGHT,
			numTriangles * 3,
			vertices,
			vertexAttrib,
			projection,
			modelView,
			frame * 0.0015,
			engine.width,
			engine.height);

		SDL_GL_SwapWindow(engine.window);

		frame += 1;
	}

	dealloc(&engine);
	return 0;
}
