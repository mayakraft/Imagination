#pragma once

#include "glad.h"
#include <stdio.h>
#include <stdlib.h>
#include "fs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ShaderProgram {
	GLuint id;
	// GLuint vbo;
	// GLuint ebo;
} ShaderProgram;

//GLuint loadShader(char *vertex_path, char *fragment_path);

void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);

// GL_FRAGMENT_SHADER 0x8B30, GL_VERTEX_SHADER 0x8B31
GLuint compileShader(unsigned short type, const GLchar* shaderSource);

GLuint createShaderProgram(const GLchar* vertex, const GLchar* fragment);

void deallocProgram(GLuint program);

GLint getAttrib(GLuint program, const char* name);
GLint getUniform(GLuint program, const char* name);

// length is the array size in bytes. for example: sizeof(GLuint) * 21 * 3
GLuint makeArrayBuffer(const void* data, size_t length);
GLuint makeElementBuffer(const void* data, size_t length);

GLuint beginVAO();
void endVAO();

#ifdef __cplusplus
}
#endif
