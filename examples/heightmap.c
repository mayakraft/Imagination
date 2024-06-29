// This demonstrates using a heightmap texture in the shader
// to create normals, affect the lighting, and displace the mesh.
// - "space" to displace the mesh in the vertical axis.
// - "r" to start and stop the camera rotation.
// even if the mesh is flat (not displaced), normal lighting still works.
#include "../include/engine.h"

#define MAX_LIGHTS 10

typedef struct Light {
	float position[3];
	float color[3];
} Light;

int main() {
	int SCREEN = 640;
	const int WIDTH = 64;
	const int HEIGHT = 64;
	GLfloat projection[16];
	GLfloat modelView[16];
	int frame = 0;
	float cameraAngle = 0;
	unsigned char isRotating = 1;
	unsigned char doDisplace = 1;

	InitParams params = {
		.flags = SDL_INIT_VIDEO,
		.title = "OpenGL Heightmap Texture",
		.width = SCREEN,
		.height = SCREEN,
	};
	GameEngine engine = initGLEngine(params);
	glDebugInfo();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	mesh_t plane;
	makeQuadPlane(&plane, WIDTH, HEIGHT);
	makePerspectiveMatrix4(45, 1.0 / 1.0, 0.1f, 1000.0f, projection);
	makeLookAtMatrix4(
		WIDTH / 2.0 + WIDTH * 1.5, HEIGHT / 2.0, WIDTH,
		WIDTH / 2.0, HEIGHT / 2.0, 0,
		0, 0, 1,
		modelView);

	Light lights[MAX_LIGHTS];
	int numLights = 2;
	lights[0] = (Light){{ 0.0f, 0.0f, 0.0f }, { 0.8f, 0.8f, 0.8f }};
	lights[1] = (Light){{ 0.0f, 0.0f, 0.0f }, { 0.8f, 0.8f, 0.8f }};

	const char *shaderPath = "./examples/shaders";
	// char *shaderPath = getMacBundleResourcesPath();
	GLuint shaderProgram = createShaderProgram(
	readFile(joinPath(shaderPath, "/heightmap.vert"), NULL),
	readFile(joinPath(shaderPath, "/heightmap.frag"), NULL));
	glUseProgram(shaderProgram);
	// free(shaderPath);

	GLint vertexAttrib = getAttrib(shaderProgram, "position");
	GLint texCoordAttrib = getAttrib(shaderProgram, "texCoord");
	GLint projectionUniform = getUniform(shaderProgram, "u_projection");
	GLint modelViewUniform = getUniform(shaderProgram, "u_modelView");
	GLint textureUniform = getUniform(shaderProgram, "u_texture");
	GLint heightmapUniform = getUniform(shaderProgram, "u_heightmap");
	GLint numLightsUniform = getUniform(shaderProgram, "u_numLights");
	GLint displaceOnUniform = getUniform(shaderProgram, "u_doDisplace");

	const char *resources = "./examples/images";
	// char *resources = getMacBundleResourcesPath();
	GLuint texture = loadGLTextureFromFileRGB(joinPath(resources, "/wall.png"));
	GLuint heightmap = loadGLTextureFromFileGrayscale(joinPath(resources, "/wall-heightmap.png"));
	// free(resources);

	glUniform1i(numLightsUniform, numLights);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureUniform, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, heightmap);
	glUniform1i(heightmapUniform, 1);

	GLuint vbo = makeArrayBuffer(plane.vertices, plane.numVertices * 3 * sizeof(GLfloat));
	GLuint tbo = makeArrayBuffer(plane.texCoords, plane.numVertices * 2 * sizeof(GLfloat));
	GLuint ebo = makeElementBuffer(plane.faces, plane.numFaces * 3 * sizeof(GLuint));

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = true; }
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_f:
						// SDL_SetWindowFullscreen(engine.window, SDL_WINDOW_FULLSCREEN);
						break;
					case SDLK_r:
						isRotating = !isRotating;
						break;
					case SDLK_SPACE:
						doDisplace = !doDisplace;
						break;
				}
			}
		}

		float time = frame / 60.0f;
		if (isRotating) {
			cameraAngle += 0.01;
			makeLookAtMatrix4(
				WIDTH / 2.0 + WIDTH * 1.5 * cos(cameraAngle),
				HEIGHT / 2.0 + HEIGHT * 1.5 * sin(cameraAngle),
				WIDTH,
				WIDTH / 2.0, HEIGHT / 2.0, 0,
				0, 0, 1,
				modelView);
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, projection);
		glUniformMatrix4fv(modelViewUniform, 1, GL_FALSE, modelView);

		// Set light position (in view space)
		lights[0].position[0] = WIDTH / 2.0 + WIDTH * 0.75 * cos(time * -0.75);
		lights[0].position[1] = HEIGHT / 2.0 + HEIGHT * 0.75 * sin(time * -0.75);
		lights[0].position[2] = WIDTH * 2;
		lights[1].position[0] = WIDTH / 2.0 + WIDTH * 0.75 * cos(time * -0.75);
		lights[1].position[1] = HEIGHT / 2.0 + HEIGHT * 0.75 * sin(time * -0.75);
		lights[1].position[2] = WIDTH;

		// Set light positions and colors
		for (int i = 0; i < numLights; i++) {
			char uniformName[256];
			// Position
			snprintf(uniformName, sizeof(uniformName), "u_lights[%d].position", i);
			GLint lightPosUniform = getUniform(shaderProgram, uniformName);
			glUniform3f(lightPosUniform, lights[i].position[0], lights[i].position[1], lights[i].position[2]);
			// Color
			snprintf(uniformName, sizeof(uniformName), "u_lights[%d].color", i);
			GLint lightColorLoc = getUniform(shaderProgram, uniformName);
			glUniform3f(lightColorLoc, lights[i].color[0], lights[i].color[1], lights[i].color[2]);
		}

		glUniform1i(displaceOnUniform, doDisplace);

		glEnableVertexAttribArray(vertexAttrib);
		glEnableVertexAttribArray(texCoordAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, tbo);
		glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, plane.numFaces * 3, GL_UNSIGNED_INT, NULL);
		glDisableVertexAttribArray(texCoordAttrib);
		glDisableVertexAttribArray(vertexAttrib);

		SDL_GL_SwapWindow(engine.window);
		frame += 1;
	}

	deallocProgram(shaderProgram);
	dealloc(&engine);
	return 0;
}
