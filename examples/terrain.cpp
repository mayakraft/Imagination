// infinite scrolling simplex noise landscape,
// vertices positions calculated on the GPU

// #ifdef _WIN32
// #define SDL_MAIN_HANDLED
// #endif

#include <string>
#include <time.h>
#include "./glm/glm.hpp"
#include "./misc/snoise.cpp"
#include "../include/engine.h"
#include "./misc/SimplexNoise.cpp"

using namespace glm;

float altitude(vec2 coord) {
	float lacunarity = 2.0;
	float persistance = 0.5;
	float noisescale = 0.005;
	float result = 0;
	for (int i = 0; i < 8; i++) {
		float lun = powf(lacunarity, i);
		float per = powf(persistance, i);
		result += snoise(coord * noisescale * lun) * per;
	}
	return result;
}

void fillTexture(unsigned char* pixels, int width, int height, unsigned int seed) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = y * width + x;
			float z = altitude(vec2(x, y));
			pixels[index * 3 + 0] = x;
			pixels[index * 3 + 1] = y;
			pixels[index * 3 + 2] = z * 64 + 127;
		}
	}
}

int main() {
	srand(time(NULL));
	// unsigned int frame = 0;
	const int WIDTH = 256;
	const int HEIGHT = 256;
	GLfloat projection[16];
	GLfloat modelView[16];
	bool arrowKeys[4] = { 0, 0, 0, 0 };
	float direction = 0;

	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "Walkable Terrain",
		.width = 640,
		.height = 640,
	};
	GameEngine engine = initGLEngine(params);
	glDebugInfo();

	mesh_t terrain;
	makeQuadPlane(&terrain, WIDTH, HEIGHT);

	unsigned char pixels[WIDTH * HEIGHT * 3];
	fillTexture(pixels, WIDTH, HEIGHT, 0);

	// for (int i = 0; i < WIDTH * HEIGHT; i++) {
	// 	printf("(%d, %d, %d), (%f, %f, %f)\n",
	// 		pixels[i * 3 + 0], pixels[i * 3 + 1], pixels[i * 3 + 2],
	// 		terrain.vertices[i * 3 + 0], terrain.vertices[i * 3 + 1], terrain.vertices[i * 3 + 2]);
	// }

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// glEnable(GL_TEXTURE);
	glEnable(GL_TEXTURE_2D);

	makePerspectiveMatrix4(45, 1.0 / 1.0, 0.1f, 1000.0f, projection);
	// makeOrthographicMatrix4(380, 380, -380, -380, 1, 2048.0, projection);
	// makeLookAtMatrix4(480, 480, 600, 0, 0, 0, 0, 0, 1, modelView);
	makeLookAtMatrix4(WIDTH * 1.5, HEIGHT * 1.5, WIDTH, WIDTH / 2.0, HEIGHT / 2.0, 0, 0, 0, 1, modelView);

	// vec2 offset = vec2(
	// 	(float)(rand() % 1000),
	// 	(float)(rand() % 1000)
	// );

	char shaderPath[] = "./examples/shaders";
	// for building in xcode
	// getMacBundleResourcesPath(shaderPath);

	std::string terrainVertexPath = std::string(shaderPath) + "/terrain.vert";
	std::string terrainFragmentPath = std::string(shaderPath) + "/terrain.frag";
	// std::string polyVertexPath = std::string(shaderPath) + "/mesh.vert";
	// std::string polyFragmentPath = std::string(shaderPath) + "/mesh.frag";

	char* terrainVertex = readFile(terrainVertexPath.c_str(), NULL);
	char* terrainFragment = readFile(terrainFragmentPath.c_str(), NULL);
	// char* polyVertex = readFile(polyVertexPath.c_str(), NULL);
	// char* polyFragment = readFile(polyFragmentPath.c_str(), NULL);

	ShaderProgram landProgram = createProgram(terrainVertex, terrainFragment);
	glUseProgram(landProgram.programID);

	GLint vertexAttrib = getAttrib(&landProgram, "position");
	GLint texLocation = getUniform(&landProgram, "tex");
	GLint widthLocation = getUniform(&landProgram, "u_width");
	GLint heightLocation = getUniform(&landProgram, "u_height");
	// GLint timeLocation = getUniform(&landProgram, "u_time");
	// GLint offsetLocation = getUniform(&landProgram, "u_offset");
	GLint projectionLocation = getUniform(&landProgram, "u_projection");
	GLint modelViewLocation = getUniform(&landProgram, "u_modelView");

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		terrain.numVertices * 3 * sizeof(GLfloat),
		terrain.vertices,
		GL_STATIC_DRAW);
	landProgram.vbo = vbo;

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		terrain.numFaces * 3 * sizeof(GLuint),
		terrain.faces,
		GL_STATIC_DRAW);
	landProgram.ebo = ibo;

	GLuint texture = loadGLTexture(pixels, WIDTH, HEIGHT, GL_RGB, GL_RGB);
	// unsigned int texture;
	// glGenTextures(1, &texture);
	// glBindTexture(GL_TEXTURE_2D, texture);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	// glGenerateMipmap(GL_TEXTURE_2D);

	// GLuint texture = loadGLTexture(pixels, WIDTH, HEIGHT);
	// glBindTexture(GL_TEXTURE_2D, texture);
	// glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);


	// ShaderProgram ballProgram = createProgram(polyVertex, polyFragment);
	// glUseProgram(ballProgram.programID);

	// GLint ballVertexAttrib = getAttrib(&ballProgram, "position");
	// GLint ballProjectionLocation = getUniform(&ballProgram, "u_projection");
	// GLint ballModelViewLocation = getUniform(&ballProgram, "u_view");

	// mesh_t icosahedron;
	// makeIcosahedron(&icosahedron);
	// // generateVertexBuffer(&ballProgram, vertices, 3 * WIDTH * HEIGHT * sizeof(GLfloat));
	// GLuint vbo2;
	// glGenBuffers(1, &vbo2);
	// glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	// glBufferData(GL_ARRAY_BUFFER, 12 * 3 * sizeof(GLfloat), icosahedron.vertices, GL_STATIC_DRAW);
	// ballProgram.vbo = vbo2;

	// // generateElementBuffer(&ballProgram, indices, numTriangles * 3);
	// GLuint ibo2;
	// glGenBuffers(1, &ibo2);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo2);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, 20 * 3 * sizeof(GLushort), icosahedron.faces, GL_STATIC_DRAW);
	// ballProgram.ebo = ibo2;

	glUseProgram(landProgram.programID);

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = true; }
			else if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
					case SDLK_LEFT: arrowKeys[3] = false; break;
					case SDLK_RIGHT: arrowKeys[1] = false; break;
					case SDLK_UP: arrowKeys[0] = false; break;
					case SDLK_DOWN: arrowKeys[2] = false; break;
					default: break;
				}
			}
			else if (e.type == SDL_KEYDOWN) {
				// printf( "%c (0x%04X)\n", (char)e.key.keysym.sym, e.key.keysym.sym );
				switch (e.key.keysym.sym) {
					case SDLK_SPACE: {
						// float z = altitude(vec2(offset[0], offset[1]));
						// if (!balls.size()) {
						// 	Ball ball = Ball();
						// 	ball.position = vec3(0, 0, z);
						// 	ball.velocity = vec3(cosf(direction), sinf(direction), 1);
						// 	balls.push_back(ball);
						// } else {
						// 	balls[0].position = vec3(0, 0, z);
						// 	balls[0].velocity = vec3(cosf(direction), sinf(direction), 1);
						// }
					}
					break;
					case SDLK_f:
					float aspect;
					setFullscreenGL(&engine, &aspect);
					makePerspectiveMatrix4(45, aspect, 0.1f, 1000.0f, projection);
					// SDL_SetWindowFullscreen(engine.window, SDL_WINDOW_FULLSCREEN);
					break;
					case SDLK_LEFT: arrowKeys[3] = true; break;
					case SDLK_RIGHT: arrowKeys[1] = true; break;
					case SDLK_UP: arrowKeys[0] = true; break;
					case SDLK_DOWN: arrowKeys[2] = true; break;
					default:
					break;
				}
			}
		}

		// for (auto ball : balls) {
		// 	float z = altitude(vec2(ball.position.x, ball.position.y));
		// 	ball.position = vec3(3, 3, z + 3);
		// 	printf("%f, %f, %f\n", ball.position.x, ball.position.y, ball.position.z);
		// }

		// for (auto ball : balls) {
		// 	float z = altitude(vec2(ball.position.x, ball.position.y));
		// 	if (ball.position.z < z) {
		// 		ball.position.z = z;
		// 		ball.velocity.z *= -1;
		// 	}
		// 	ball.velocity += vec3(0, 0, -0.1);
		// 	ball.position += ball.velocity;
		// 	// printf("%f, %f, %f\n", ball.position.x, ball.position.y, ball.position.z);
		// }

		// float walkSpeed = 0.2f;
		// if (arrowKeys[0]) {
		// 	offset[0] += walkSpeed * cos(direction);
		// 	offset[1] += walkSpeed * sin(direction);
		// }
		// if (arrowKeys[2]) {
		// 	offset[0] -= walkSpeed * cos(direction);
		// 	offset[1] -= walkSpeed * sin(direction);
		// }
		// if (arrowKeys[1]) {
		// 	direction -= 0.05;
		// }
		// if (arrowKeys[3]) {
		// 	direction += 0.05;
		// }

		// float lookAtPoint[] = { cos(direction), sin(direction) };
		// float z = 3 + altitude(vec2(offset[0], offset[1]));
		// makeLookAtMatrix4(
		// 	0, 0, z,
		// 	lookAtPoint[0], lookAtPoint[1], z,
		// 	0, 0, 1,
		// 	modelView);

		float time = 0;
		// float time = frame * (1.0 / 60.0);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glUseProgram(landProgram.programID);
		glEnableVertexAttribArray(vertexAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, landProgram.vbo);
		glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		// glUniform1f(timeLocation, time);
		// glUniform2f(offsetLocation, offset[0], offset[1]);
		glUniform1f(widthLocation, (float)WIDTH);
		glUniform1f(heightLocation, (float)HEIGHT);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projection);
		glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(texLocation, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, landProgram.ebo);
		glDrawElements(GL_TRIANGLES, terrain.numFaces * 3, GL_UNSIGNED_INT, NULL);
		glDisableVertexAttribArray(vertexAttrib);

		// glUseProgram(ballProgram.programID);
		// glEnableVertexAttribArray(ballVertexAttrib);
		// glBindBuffer(GL_ARRAY_BUFFER, ballProgram.vbo);
		// glVertexAttribPointer(ballVertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		// // glUniform1f(timeLocation, time);
		// // glUniform2f(offsetLocation, offset[0], offset[1]);
		// glUniformMatrix4fv(ballProjectionLocation, 1, GL_FALSE, projection);
		// glUniformMatrix4fv(ballModelViewLocation, 1, GL_FALSE, modelView);
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ballProgram.ebo);
		// glDrawElements(GL_TRIANGLES, 20 * 3, GL_UNSIGNED_SHORT, NULL);
		// glDisableVertexAttribArray(ballVertexAttrib);

		glUseProgram(0);

		SDL_GL_SwapWindow(engine.window);

		// frame += 1;
	}

	deallocProgram(&landProgram);
	// deallocProgram(&ballProgram);
	dealloc(&engine);
	return 0;
}
