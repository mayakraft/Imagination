// This demonstrates
// - using a heightmap texture in the shader
// - sending multiple textures to shaders
// to create normals, affect the lighting, and displace the mesh.
// - "space" to displace the mesh in the vertical axis.
// - "r" to start and stop the camera rotation.
// - "s" to toggle smooth texture or not
// even if the mesh is flat (not displaced), normal lighting still works.
#include "../include/engine.h"

#define MAX_LIGHTS 10

typedef struct Light {
	float position[3];
	float color[3];
} Light;

int main() {
	int SCREEN = 640;
	const int WIDTH = 32;
	const int HEIGHT = 32;
	GLfloat projection[16];
	GLfloat view[16];
	GLfloat model[16];
	int frame = 0;
	float cameraAngle = 0;
	unsigned char isRotating = 1;
	unsigned char doDisplace = 1;
	unsigned char useSmooth = 1;
	setIdentity4(model);

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
	makeUVPlane(&plane, WIDTH, HEIGHT);
	makePerspectiveMatrix4(45, 2.0 / 1.0, 0.1f, 1000.0f, projection);
	makeLookAtMatrix4(
		WIDTH / 2.0 + WIDTH * 1.5, HEIGHT / 2.0, WIDTH,
		WIDTH / 2.0, HEIGHT / 2.0, 0,
		0, 0, 1,
		view);

	Light lights[MAX_LIGHTS];
	int numLights = 2;
	lights[0] = (Light){{ 0.0f, 0.0f, 0.0f }, { 0.8f, 0.8f, 0.8f }};
	lights[1] = (Light){{ 0.0f, 0.0f, 0.0f }, { 0.8f, 0.8f, 0.8f }};

	GLuint vbo = makeArrayBuffer(plane.vertices, plane.numVertices * 3 * sizeof(GLfloat));
	GLuint tbo = makeArrayBuffer(plane.texCoords, plane.numVertices * 2 * sizeof(GLfloat));
	GLuint ebo = makeElementBuffer(plane.faces, plane.numFaces * 3 * sizeof(GLuint));

	const char *resources = "./examples/images";
	// char *resources = getMacBundleResourcesPath();
	GLuint texture = loadGLTextureFromFileRGBWithBlend(
		joinPath(resources, "/wall.png"),
		GL_NEAREST);
	GLuint textureSmooth = loadGLTextureFromFileRGBWithBlend(
		joinPath(resources, "/wall.png"),
		GL_LINEAR);
	GLuint heightmap = loadGLTextureFromFileGrayscaleWithBlend(
		joinPath(resources, "/wall-heightmap.png"),
		GL_NEAREST);
	GLuint heightmapSmooth = loadGLTextureFromFileGrayscaleWithBlend(
		joinPath(resources, "/wall-heightmap.png"),
		GL_LINEAR);
	// free(resources);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // blend
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	const char *shaderPath = "./examples/shaders";
	// char *shaderPath = getMacBundleResourcesPath();
	GLuint programHeightmap = createShaderProgram(
		readFile(joinPath(shaderPath, "/heightmap.vert"), NULL),
		readFile(joinPath(shaderPath, "/heightmap.frag"), NULL));

	GLuint programDisplace = createShaderProgram(
		readFile(joinPath(shaderPath, "/heightmap-displace.vert"), NULL),
		readFile(joinPath(shaderPath, "/heightmap-displace.frag"), NULL));

	glUseProgram(programHeightmap);
	GLint vertexAttribHeightmap = getAttrib(programHeightmap, "aPosition");
	GLint texCoordAttribHeightmap = getAttrib(programHeightmap, "aTexCoord");
	glUniform1i(glGetUniformLocation(programHeightmap, "u_numLights"), numLights);
	glUniform3f(glGetUniformLocation(programHeightmap, "u_ambientColor"), 0.1, 0.1, 0.1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, useSmooth ? textureSmooth : texture);
	glUniform1i(glGetUniformLocation(programHeightmap, "u_texture"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, useSmooth ? heightmapSmooth : heightmap);
	glUniform1i(glGetUniformLocation(programHeightmap, "u_heightmap"), 1);

	glUseProgram(programDisplace);
	GLint vertexAttribDisplace = getAttrib(programDisplace, "aPosition");
	GLint texCoordAttribDisplace = getAttrib(programDisplace, "aTexCoord");
	glUniform1i(glGetUniformLocation(programDisplace, "u_numLights"), numLights);
	glUniform3f(glGetUniformLocation(programDisplace, "u_ambientColor"), 0.1, 0.1, 0.1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, useSmooth ? textureSmooth : texture);
	glUniform1i(glGetUniformLocation(programDisplace, "u_texture"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, useSmooth ? heightmapSmooth : heightmap);
	glUniform1i(glGetUniformLocation(programDisplace, "u_heightmap"), 1);

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
					case SDLK_s:
						useSmooth = !useSmooth;

						glUseProgram(programHeightmap);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, useSmooth ? textureSmooth : texture);
						glUniform1i(glGetUniformLocation(programHeightmap, "u_texture"), 0);
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, useSmooth ? heightmapSmooth : heightmap);
						glUniform1i(glGetUniformLocation(programHeightmap, "u_heightmap"), 1);

						glUseProgram(programDisplace);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, useSmooth ? textureSmooth : texture);
						glUniform1i(glGetUniformLocation(programDisplace, "u_texture"), 0);
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, useSmooth ? heightmapSmooth : heightmap);
						glUniform1i(glGetUniformLocation(programDisplace, "u_heightmap"), 1);
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
				WIDTH / 2.0 + WIDTH * 1 * cos(cameraAngle),
				HEIGHT / 2.0 + HEIGHT * 1 * sin(cameraAngle),
				WIDTH,
				WIDTH / 2.0, HEIGHT / 2.0, 0,
				0, 0, 1,
				view);
		}
		// Set light position (in view space)
		lights[0].position[0] = WIDTH / 2.0 + WIDTH * 0.75 * cos(time * -0.75);
		lights[0].position[1] = HEIGHT / 2.0 + HEIGHT * 0.75 * sin(time * -0.75);
		lights[0].position[2] = WIDTH / 2.0; // * 2;
		lights[1].position[0] = WIDTH / 2.0 + WIDTH * 0.75 * cos(time * -0.75);
		lights[1].position[1] = HEIGHT / 2.0 + HEIGHT * 0.75 * sin(time * -0.75);
		lights[1].position[2] = WIDTH;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, engine.width, engine.height * 0.5);
		glUseProgram(programDisplace);
		glUniformMatrix4fv(glGetUniformLocation(programDisplace, "u_projection"), 1, GL_FALSE, projection);
		glUniformMatrix4fv(glGetUniformLocation(programDisplace, "u_view"), 1, GL_FALSE, view);
		glUniformMatrix4fv(glGetUniformLocation(programDisplace, "u_model"), 1, GL_FALSE, model);
		glUniform1i(glGetUniformLocation(programDisplace, "u_doDisplace"), doDisplace);
		// Set light positions and colors
		for (int i = 0; i < numLights; i++) {
			char uniformName[256];
			// Position
			snprintf(uniformName, sizeof(uniformName), "u_lights[%d].position", i);
			GLint lightPosUniform = getUniform(programDisplace, uniformName);
			glUniform3f(lightPosUniform, lights[i].position[0], lights[i].position[1], lights[i].position[2]);
			// Color
			snprintf(uniformName, sizeof(uniformName), "u_lights[%d].color", i);
			GLint lightColorLoc = getUniform(programDisplace, uniformName);
			glUniform3f(lightColorLoc, lights[i].color[0], lights[i].color[1], lights[i].color[2]);
		}
		glEnableVertexAttribArray(vertexAttribDisplace);
		glEnableVertexAttribArray(texCoordAttribDisplace);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(vertexAttribDisplace, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, tbo);
		glVertexAttribPointer(texCoordAttribDisplace, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, plane.numFaces * 3, GL_UNSIGNED_INT, NULL);
		glDisableVertexAttribArray(texCoordAttribDisplace);
		glDisableVertexAttribArray(vertexAttribDisplace);

		glViewport(0, engine.height * 0.5, engine.width, engine.height * 0.5);
		glUseProgram(programHeightmap);
		glUniformMatrix4fv(glGetUniformLocation(programHeightmap, "u_projection"), 1, GL_FALSE, projection);
		glUniformMatrix4fv(glGetUniformLocation(programHeightmap, "u_view"), 1, GL_FALSE, view);
		glUniformMatrix4fv(glGetUniformLocation(programHeightmap, "u_model"), 1, GL_FALSE, model);
		glUniform1i(glGetUniformLocation(programHeightmap, "u_doDisplace"), doDisplace);
		// Set light positions and colors
		for (int i = 0; i < numLights; i++) {
			char uniformName[256];
			// Position
			snprintf(uniformName, sizeof(uniformName), "u_lights[%d].position", i);
			GLint lightPosUniform = getUniform(programHeightmap, uniformName);
			glUniform3f(lightPosUniform, lights[i].position[0], lights[i].position[1], lights[i].position[2]);
			// Color
			snprintf(uniformName, sizeof(uniformName), "u_lights[%d].color", i);
			GLint lightColorLoc = getUniform(programHeightmap, uniformName);
			glUniform3f(lightColorLoc, lights[i].color[0], lights[i].color[1], lights[i].color[2]);
		}
		glEnableVertexAttribArray(vertexAttribHeightmap);
		glEnableVertexAttribArray(texCoordAttribHeightmap);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(vertexAttribHeightmap, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, tbo);
		glVertexAttribPointer(texCoordAttribHeightmap, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, plane.numFaces * 3, GL_UNSIGNED_INT, NULL);
		glDisableVertexAttribArray(texCoordAttribHeightmap);
		glDisableVertexAttribArray(vertexAttribHeightmap);

		SDL_GL_SwapWindow(engine.window);
		frame += 1;
	}

	deallocProgram(programDisplace);
	dealloc(&engine);
	return 0;
}
