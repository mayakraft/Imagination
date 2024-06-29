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

void setupFramebuffer(
	int width,
	int height,
	GLuint *framebuffer,
	GLuint *texColorBuffer,
	GLuint *rbo
) {
	// Framebuffer
	glGenFramebuffers(1, framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);

	// Create a texture to render to
	glGenTextures(1, texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, *texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texColorBuffer, 0);

	// Create a renderbuffer object for depth and stencil attachment (optional)
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Error framebuffer is not complete\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int main(void) {
	int SCREEN = 640;
	int frame = 0;
	unsigned char showBuffer = 1;
	GLfloat projection[16];
	GLfloat view[16];
	GLfloat model[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	float quadVertices[] = {
		// pos / texCoords
		-1,  1,  0, 1,
		-1, -1,  0, 0,
		 1, -1,  1, 0,
		-1,  1,  0, 1,
		 1, -1,  1, 0,
		 1,  1,  1, 1
	};
	mesh_t polyhedron;
	makeDodecahedron(&polyhedron);
	makePerspectiveMatrix4(45, 1.0 / 1.0, 0.1f, 100.0f, projection);
	// makeOrthographicMatrix4(380, 380, -380, -380, 1, 2048.0, projection);
	makeLookAtMatrix4(4, 0, 0, 0, 0, 0, 0, 0, 1, view);

	InitParams params = {
		.flags = SDL_INIT_VIDEO,
		.title = "Post-processing shader",
		.width = SCREEN,
		.height = SCREEN
	};
	GameEngine engine = initGLEngine(params);
	glDebugInfo();

	char shaderPath[256] = "./examples/shaders";
	// char shaderPath[256] = getMacBundleResourcesPath();
	GLuint meshProgram = createShaderProgram(
		readFile(joinPath(shaderPath, "/mesh.vert"), NULL),
		readFile(joinPath(shaderPath, "/mesh.frag"), NULL));
		// readFile(joinPath(shaderPath, "/mesh-flat-shading.frag"), NULL));
	GLuint postProcessProgram = createShaderProgram(
		readFile(joinPath(shaderPath, "/post-process.vert"), NULL),
		readFile(joinPath(shaderPath, "/post-process.frag"), NULL));

	// setup attribs and uniforms
	glUseProgram(meshProgram);
	GLint meshVertexAttrib = getAttrib(meshProgram, "position");
	GLint meshNormalAttrib = getAttrib(meshProgram, "normal");
	GLint meshProjectionUniform = getUniform(meshProgram, "u_projection");
	GLint meshViewUniform = getUniform(meshProgram, "u_view");
	GLint meshModelUniform = getUniform(meshProgram, "u_model");
	GLint meshObjectColorUniform = getUniform(meshProgram, "u_materialColor");
	GLint meshAmbientLightUniform = getUniform(meshProgram, "u_ambientColor");
	GLint meshNumLightsUniform = getUniform(meshProgram, "u_numLights");
	// GLint meshLightsUniform = getUniform(meshProgram, "u_lights");
	GLuint meshVBO_v = makeArrayBuffer(polyhedron.vertices,
		polyhedron.numVertices * 3 * sizeof(GLfloat));
	GLuint meshVBO_n = makeArrayBuffer(polyhedron.vertices,
		polyhedron.numVertices * 3 * sizeof(GLfloat));
	GLuint meshEBO = makeElementBuffer(polyhedron.faces,
		polyhedron.numFaces * 3 * sizeof(GLuint));

	Light lights[MAX_LIGHTS];
	int numLights = 4;
	lights[0] = (Light){{ 4.0f, 4.0f, 4.0f }, { 0.7f, 0.9f, 0.9f }};
	lights[1] = (Light){{ -2.0f, -2.0f, -2.0f }, { 0.8f, 0.9f, 0.8f }};
	lights[2] = (Light){{ -3.0f, 3.0f, -3.0f }, { 0.9f, 0.8f, 0.8f }};
	lights[3] = (Light){{ 2.0f, -2.0f, 2.0f }, { 0.9f, 0.7f, 0.9f }};

	// Set light positions and colors
	for (int i = 0; i < numLights; i++) {
		float *p = lights[i].position;
		float *c = lights[i].color;
		char uniformName[256];
		// Position
		snprintf(uniformName, sizeof(uniformName), "u_lights[%d].position", i);
		GLint lightPosUniform = getUniform(meshProgram, uniformName);
		glUniform3f(lightPosUniform, p[0], p[1], p[2]);
		// Color
		snprintf(uniformName, sizeof(uniformName), "u_lights[%d].color", i);
		GLint lightColorUniform = getUniform(meshProgram, uniformName);
		glUniform3f(lightColorUniform, c[0], c[1], c[2]);
	}

	// Set the number of lights
	glUniform1i(meshNumLightsUniform, numLights);
	glUniform3f(meshAmbientLightUniform, 0.7, 0.7, 0.7);
	glUniform3f(meshObjectColorUniform, 0.15, 0.3, 0.75);


	// Use mesh shader program
	GLuint meshVAO = beginVAO();
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO_v);
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO_n);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
	glEnableVertexAttribArray(meshVertexAttrib);
	glEnableVertexAttribArray(meshNormalAttrib);
	glVertexAttribPointer(meshVertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(meshNormalAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	endVAO();

	GLuint framebuffer;
	GLuint texColorBuffer;
	GLuint rbo;
	setupFramebuffer(
		SCREEN * engine.pixelScale,
		SCREEN * engine.pixelScale,
		&framebuffer,
		&texColorBuffer,
		&rbo);

	// setup attribs and uniforms
	glUseProgram(postProcessProgram);
	GLint postProcessVertexAttrib = getAttrib(postProcessProgram, "aPos");
	GLint postProcessTexCoordsAttrib = getAttrib(postProcessProgram, "aTexCoords");
	GLint postProcessScreenTextureUniform = getUniform(postProcessProgram, "screenTexture");

	GLuint quadVAO = beginVAO();
	GLuint quadVBO = makeArrayBuffer(quadVertices, sizeof(quadVertices));
	glEnableVertexAttribArray(postProcessVertexAttrib);
	glEnableVertexAttribArray(postProcessTexCoordsAttrib);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);
	endVAO();

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
		// makeLookAtMatrix4(
		// 	4 * cos(time), 4 * sin(time), 1*sin(time),
		// 	0, 0, 0,
		// 	0, 0, 1,
		// 	view);
		makeMatrix4RotateZ(model, time);

		// render scene to framebuffer
		if (showBuffer) {
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		}

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use mesh shader program
		glUseProgram(meshProgram);
		glUniformMatrix4fv(meshProjectionUniform, 1, GL_FALSE, projection);
		glUniformMatrix4fv(meshViewUniform, 1, GL_FALSE, view);
		glUniformMatrix4fv(meshModelUniform, 1, GL_FALSE, model);
		// glUniform3f(meshLightPosUniform, 3, 3, 3);
		// glUniform3f(meshLightColorUniform, 1, 1, 1);
		// glUniform3f(meshObjectColorUniform, 0.1, 0.3, 0.7);
		glBindVertexArray(meshVAO);
		glDrawElements(GL_TRIANGLES, polyhedron.numFaces * 3, GL_UNSIGNED_INT, NULL);
		// glBindVertexArray(0);

		if (showBuffer) {
			// 2. Render framebuffer texture to screen with post-processing shader
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT);

			// Use post-processing shader program
			glUseProgram(postProcessProgram);
			glBindVertexArray(quadVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			// Set the uniform to use the texture unit 0
			glUniform1i(postProcessScreenTextureUniform, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glUseProgram(0);

		SDL_GL_SwapWindow(engine.window);

		frame++;
	}

	// glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &texColorBuffer);
	glDeleteRenderbuffers(1, &rbo);

	// deallocProgram(postProcessProgram);
	deallocProgram(meshProgram);
	dealloc(&engine);
	return 0;
}
