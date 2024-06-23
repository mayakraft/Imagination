// this demonstrates using multiple shaders,
// rendering one to a frame buffer and
// using another shader for post-processing effects
// spacebar to toggle between off and on
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "../include/engine.h"

ShaderProgram loadShader(
	const char* directory,
	const char* vertexFilename,
	const char* fragmentFilename) {
	// for building in xcode
	// getMacBundleResourcesPath(directory);
	char vertexFullPath[512], fragmentFullPath[512];
	strcpy(vertexFullPath, directory);
	strcat(vertexFullPath, vertexFilename);
	strcpy(fragmentFullPath, directory);
	strcat(fragmentFullPath, fragmentFilename);

	char* vertex = readFile(vertexFullPath);
	char* fragment = readFile(fragmentFullPath);

	ShaderProgram postProcessProgram = createProgram(vertex, fragment);
	glUseProgram(postProcessProgram.programID);
	free(vertex);
	free(fragment);
	return postProcessProgram;
}

void setupFramebuffer(int width, int height, GLuint *framebuffer, GLuint *texColorBuffer, GLuint *rbo) {
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
		printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int main(void) {
	int SCREEN = 640;
	int frame = 0;
	unsigned char showBuffer = 1;
	GLfloat projection[16];
	GLfloat modelView[16];
	GLfloat model[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	// GLfloat screenCoords[] = { -1, -1, 1, -1, 1, 1, -1, 1 };
	// GLuint screenCoordIndices[] = { 0, 1, 2, 3 };
	mesh_t polyhedron;
	makeDodecahedron(&polyhedron);
	makePerspectiveMatrix4(45, 1.0 / 1.0, 0.1f, 100.0f, projection);
	// makeOrthographicMatrix4(380, 380, -380, -380, 1, 2048.0, projection);
	float quadVertices[] = {
		// pos / texCoords
		-1,  1,  0, 1,
		-1, -1,  0, 0,
		 1, -1,  1, 0,
		-1,  1,  0, 1,
		 1, -1,  1, 0,
		 1,  1,  1, 1
	};

	InitParams params;
	params.flags = SDL_INIT_VIDEO;
	params.title = "Post-processing shader";
	params.width = SCREEN;
	params.height = SCREEN;
	GameEngine engine = initGLEngine(params);

	glDebugInfo();

	char shaderPath[256] = "./examples/shaders";
	// for building in xcode
	// getMacBundleResourcesPath(shaderPath);

	ShaderProgram meshProgram = loadShader(
		shaderPath,
		"/mesh.vert",
		"/mesh.frag");
	ShaderProgram postProcessProgram = loadShader(
		shaderPath,
		"/post-process.vert",
		"/post-process.frag");

	// setup attribs and uniforms
	glUseProgram(meshProgram.programID);
	GLint meshVertexAttrib = getAttrib(&meshProgram, "position");
	GLint meshNormalAttrib = getAttrib(&meshProgram, "normal");
	GLint meshProjectionUniform = getUniform(&meshProgram, "u_projection");
	GLint meshViewUniform = getUniform(&meshProgram, "u_view");
	GLint meshModelUniform = getUniform(&meshProgram, "u_model");
	GLint meshLightPosUniform = getUniform(&meshProgram, "u_lightPos");
	GLint meshLightColorUniform = getUniform(&meshProgram, "u_lightColor");
	GLint meshObjectColorUniform = getUniform(&meshProgram, "u_objectColor");
	GLuint meshVBO_v, meshVBO_n;
	glGenBuffers(1, &meshVBO_v);
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO_v);
	glBufferData(
		GL_ARRAY_BUFFER,
		polyhedron.numVertices * 3 * sizeof(GLfloat),
		polyhedron.vertices,
		GL_STATIC_DRAW);
	glGenBuffers(1, &meshVBO_n);
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO_n);
	glBufferData(
		GL_ARRAY_BUFFER,
		polyhedron.numVertices * 3 * sizeof(GLfloat),
		polyhedron.vertices,
		GL_STATIC_DRAW);
	GLuint meshEBO;
	glGenBuffers(1, &meshEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, polyhedron.numFaces * 3 * sizeof(GLushort), polyhedron.faces, GL_STATIC_DRAW);
	meshProgram.ebo = meshEBO;

	GLuint framebuffer;
	GLuint texColorBuffer;
	GLuint rbo;
	setupFramebuffer(SCREEN * engine.pixelScale, SCREEN * engine.pixelScale, &framebuffer, &texColorBuffer, &rbo);

	// setup attribs and uniforms
	glUseProgram(postProcessProgram.programID);
	// GLint postProcessVertexAttrib = getAttrib(&postProcessProgram, "aPos");
	// GLint postProcessTexCoordsAttrib = getAttrib(&postProcessProgram, "aTexCoords");
	// GLint postProcessScreenTextureUniform = getUniform(&postProcessProgram, "screenTexture");

	GLuint quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

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
		makeLookAtMatrix4(
			4 * cos(time), 4 * sin(time), 1*sin(time),
			0, 0, 0,
			0, 0, 1,
			modelView);

		if (showBuffer) {
			// mesh
			// 1. Render scene to framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glEnable(GL_DEPTH_TEST);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		} else {
			glEnable(GL_DEPTH_TEST);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		// Use mesh shader program
		glUseProgram(meshProgram.programID);
		glBindBuffer(GL_ARRAY_BUFFER, meshVBO_v);
		glBindBuffer(GL_ARRAY_BUFFER, meshVBO_n);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshProgram.ebo);
		glEnableVertexAttribArray(meshVertexAttrib);
		glEnableVertexAttribArray(meshNormalAttrib);
		glVertexAttribPointer(meshVertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glVertexAttribPointer(meshNormalAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glUniformMatrix4fv(meshProjectionUniform, 1, GL_FALSE, projection);
		glUniformMatrix4fv(meshViewUniform, 1, GL_FALSE, modelView);
		glUniformMatrix4fv(meshModelUniform, 1, GL_FALSE, model);
		glUniform3f(meshLightPosUniform, 3, 3, 3);
		glUniform3f(meshLightColorUniform, 1, 1, 1);
		glUniform3f(meshObjectColorUniform, 0.1, 0.3, 0.7);
		glDrawElements(GL_TRIANGLES, polyhedron.numFaces * 3, GL_UNSIGNED_SHORT, NULL);
		glDisableVertexAttribArray(meshNormalAttrib);
		glDisableVertexAttribArray(meshVertexAttrib);


		if (showBuffer) {
			// 2. Render framebuffer texture to screen with post-processing shader
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT);

			// Use post-processing shader program
			glUseProgram(postProcessProgram.programID);
			glActiveTexture(GL_TEXTURE0); // Activate the texture unit first
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			// Set the uniform to use the texture unit 0
			glUniform1i(glGetUniformLocation(postProcessProgram.programID, "screenTexture"), 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// post processing
		// glUseProgram(postProcessProgram.programID);
		// glBindBuffer(GL_ARRAY_BUFFER, postProcessProgram.vbo);
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, postProcessProgram.ebo);
		// glEnableVertexAttribArray(postProcessVertexAttrib);
		// glVertexAttribPointer(postProcessVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		// glUniform1f(postProcessTimeUniform, time);
		// glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);
		// glDisableVertexAttribArray(postProcessVertexAttrib);

		glUseProgram(0);

		SDL_GL_SwapWindow(engine.window);

		frame++;
	}


	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &texColorBuffer);
	glDeleteRenderbuffers(1, &rbo);

	// deallocProgram(&postProcessProgram);
	deallocProgram(&meshProgram);
	dealloc(&engine);
	return 0;
}
