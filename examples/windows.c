// this demonstrates using multiple shaders,
// rendering one to a frame buffer and
// using another shader for post-processing effects
// spacebar to toggle between off and on
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "../include/engine.h"

#define MAX_LIGHTS 10

typedef struct Light {
	float position[3];
	float color[3];
} Light;

int main(void) {
	int SCREEN = 640;
	int frame = 0;
	unsigned char showBuffer = 1;
	GLfloat projection[16];
	GLfloat view[16];
	GLfloat model[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	mesh_t polyhedron, quad;
	makePerspectiveMatrix4(45, 1.0 / 1.0, 0.1f, 100.0f, projection);
	// makeOrthographicMatrix4(380, 380, -380, -380, 1, 2048.0, projection);
	makeLookAtMatrix4(3, 0, 0, 0, 0, 0, 0, 0, 1, view);
	makeDodecahedron(&polyhedron);
	makeQuadPlane(&quad, 1, 1);

	Light lights[MAX_LIGHTS];
	int numLights = 4;
	lights[0] = (Light){{ 4.0f, 4.0f, 4.0f }, { 0.7f, 0.9f, 0.9f }};
	lights[1] = (Light){{ -2.0f, -2.0f, -2.0f }, { 0.8f, 0.9f, 0.8f }};
	lights[2] = (Light){{ -3.0f, 3.0f, -3.0f }, { 0.9f, 0.8f, 0.8f }};
	lights[3] = (Light){{ 2.0f, -2.0f, 2.0f }, { 0.9f, 0.7f, 0.9f }};

	InitParams params = {
		.flags = SDL_INIT_VIDEO,
		.title = "Viewport Windows",
		.width = SCREEN,
		.height = SCREEN
	};
	GameEngine engine = initGLEngine(params);
	glDebugInfo();

	char shaderPath[256] = "./examples/shaders";
	// char shaderPath[256] = getMacBundleResourcesPath();
	GLuint meshProgram1 = createShaderProgram(
		readFile(joinPath(shaderPath, "/mesh.vert"), NULL),
		readFile(joinPath(shaderPath, "/mesh.frag"), NULL));
	GLuint meshProgram4 = createShaderProgram(
		readFile(joinPath(shaderPath, "/mesh.vert"), NULL),
		readFile(joinPath(shaderPath, "/mesh-normals.frag"), NULL));
	GLuint meshProgram3 = createShaderProgram(
		readFile(joinPath(shaderPath, "/mesh.vert"), NULL),
		readFile(joinPath(shaderPath, "/mesh-flat-shading-normals.frag"), NULL));
	GLuint meshProgram2 = createShaderProgram(
		readFile(joinPath(shaderPath, "/mesh.vert"), NULL),
		readFile(joinPath(shaderPath, "/mesh-flat-shading.frag"), NULL));

	// setup attribs and uniforms
	glUseProgram(meshProgram1);
	GLint mesh1VertexAttrib = getAttrib(meshProgram1, "position");
	GLint mesh1NormalAttrib = getAttrib(meshProgram1, "normal");

	GLuint meshVBO_v = makeArrayBuffer(polyhedron.vertices,
		polyhedron.numVertices * 3 * sizeof(GLfloat));
	GLuint meshVBO_n = makeArrayBuffer(polyhedron.vertices,
		polyhedron.numVertices * 3 * sizeof(GLfloat));
	GLuint meshEBO = makeElementBuffer(polyhedron.faces,
		polyhedron.numFaces * 3 * sizeof(GLuint));

	// Use mesh shader program
	GLuint meshVAO = beginVAO();
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO_v);
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO_n);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
	glEnableVertexAttribArray(mesh1VertexAttrib);
	glEnableVertexAttribArray(mesh1NormalAttrib);
	glVertexAttribPointer(mesh1VertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(mesh1NormalAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	endVAO();

	glUseProgram(meshProgram1);
	glUniformMatrix4fv(glGetUniformLocation(meshProgram1, "u_projection"), 1, GL_FALSE, projection);
	glUniformMatrix4fv(glGetUniformLocation(meshProgram1, "u_view"), 1, GL_FALSE, view);
	glUseProgram(meshProgram2);
	glUniformMatrix4fv(glGetUniformLocation(meshProgram2, "u_projection"), 1, GL_FALSE, projection);
	glUniformMatrix4fv(glGetUniformLocation(meshProgram2, "u_view"), 1, GL_FALSE, view);
	glUseProgram(meshProgram3);
	glUniformMatrix4fv(glGetUniformLocation(meshProgram3, "u_projection"), 1, GL_FALSE, projection);
	glUniformMatrix4fv(glGetUniformLocation(meshProgram3, "u_view"), 1, GL_FALSE, view);
	glUseProgram(meshProgram4);
	glUniformMatrix4fv(glGetUniformLocation(meshProgram4, "u_projection"), 1, GL_FALSE, projection);
	glUniformMatrix4fv(glGetUniformLocation(meshProgram4, "u_view"), 1, GL_FALSE, view);

	glUseProgram(meshProgram1);
	// Set light positions and colors
	for (int i = 0; i < numLights; i++) {
		float *p = lights[i].position;
		float *c = lights[i].color;
		char uniformName[256];
		// Position
		snprintf(uniformName, sizeof(uniformName), "u_lights[%d].position", i);
		GLint lightPosUniform = getUniform(meshProgram1, uniformName);
		glUniform3f(lightPosUniform, p[0], p[1], p[2]);
		// Color
		snprintf(uniformName, sizeof(uniformName), "u_lights[%d].color", i);
		GLint lightColorUniform = getUniform(meshProgram1, uniformName);
		glUniform3f(lightColorUniform, c[0], c[1], c[2]);
	}
	glUseProgram(meshProgram2);
	// Set light positions and colors
	for (int i = 0; i < numLights; i++) {
		float *p = lights[i].position;
		float *c = lights[i].color;
		char uniformName[256];
		// Position
		snprintf(uniformName, sizeof(uniformName), "u_lights[%d].position", i);
		GLint lightPosUniform = getUniform(meshProgram2, uniformName);
		glUniform3f(lightPosUniform, p[0], p[1], p[2]);
		// Color
		snprintf(uniformName, sizeof(uniformName), "u_lights[%d].color", i);
		GLint lightColorUniform = getUniform(meshProgram2, uniformName);
		glUniform3f(lightColorUniform, c[0], c[1], c[2]);
	}

	// Set the number of lights
	glUseProgram(meshProgram1);
	glUniform1i(glGetUniformLocation(meshProgram1, "u_numLights"), numLights);
	glUniform3f(glGetUniformLocation(meshProgram1, "u_ambientColor"), 0.7, 0.7, 0.7);
	glUniform3f(glGetUniformLocation(meshProgram1, "u_materialColor"), 0.15, 0.3, 0.75);
	glUseProgram(meshProgram2);
	glUniform1i(glGetUniformLocation(meshProgram2, "u_numLights"), numLights);
	glUniform3f(glGetUniformLocation(meshProgram2, "u_ambientColor"), 0.7, 0.7, 0.7);
	glUniform3f(glGetUniformLocation(meshProgram2, "u_materialColor"), 0.15, 0.3, 0.75);

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
			if (e.type == SDL_KEYDOWN) {
				// printf( "%c (0x%04X)\n", (char)e.key.keysym.sym, e.key.keysym.sym );
				switch (e.key.keysym.sym) {
					case SDLK_SPACE: showBuffer = !showBuffer; break;
				}
			}
		}

		float time = frame * 0.01;
		makeMatrix4RotateZ(model, time);

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bottom left
		glViewport(0, 0, engine.width * 0.5, engine.width * 0.5);
		glUseProgram(meshProgram1);
		glUniformMatrix4fv(glGetUniformLocation(meshProgram1, "u_model"), 1, GL_FALSE, model);
		glBindVertexArray(meshVAO);
		glDrawElements(GL_TRIANGLES, polyhedron.numFaces * 3, GL_UNSIGNED_INT, NULL);

		// top left
		glViewport(0, engine.width * 0.5, engine.width * 0.5, engine.width * 0.5);
		glUseProgram(meshProgram2);
		glUniformMatrix4fv(glGetUniformLocation(meshProgram2, "u_model"), 1, GL_FALSE, model);
		glBindVertexArray(meshVAO);
		glDrawElements(GL_TRIANGLES, polyhedron.numFaces * 3, GL_UNSIGNED_INT, NULL);

		// top right
		glViewport(engine.width * 0.5, engine.width * 0.5, engine.width * 0.5, engine.width * 0.5);
		glUseProgram(meshProgram3);
		glUniformMatrix4fv(glGetUniformLocation(meshProgram3, "u_model"), 1, GL_FALSE, model);
		glBindVertexArray(meshVAO);
		glDrawElements(GL_TRIANGLES, polyhedron.numFaces * 3, GL_UNSIGNED_INT, NULL);

		// bottom right
		glViewport(engine.width * 0.5, 0, engine.width * 0.5, engine.width * 0.5);
		glUseProgram(meshProgram4);
		glUniformMatrix4fv(glGetUniformLocation(meshProgram4, "u_model"), 1, GL_FALSE, model);
		glBindVertexArray(meshVAO);
		glDrawElements(GL_TRIANGLES, polyhedron.numFaces * 3, GL_UNSIGNED_INT, NULL);

		SDL_GL_SwapWindow(engine.window);

		frame++;
	}

	// deallocProgram(postProcessProgram);
	deallocProgram(meshProgram1);
	dealloc(&engine);
	return 0;
}
