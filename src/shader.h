#pragma once

#include "glad.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ShaderProgram {
	GLuint programID;
	GLuint vbo;
	GLuint ebo;
} ShaderProgram;

// make sure to free the returned value after you are done using it
char *readFile(const char *filename);

//GLuint loadShader(char *vertex_path, char *fragment_path);

void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);

// GL_FRAGMENT_SHADER 0x8B30, GL_VERTEX_SHADER 0x8B31
GLuint compileShader(unsigned short type, const GLchar* shaderSource);

ShaderProgram createProgram(const GLchar* vertex, const GLchar* fragment);

void deallocProgram(ShaderProgram *program);

GLint getAttrib(ShaderProgram *program, const char* name);
GLint getUniform(ShaderProgram *program, const char* name);

GLuint generateVertexBuffer(const void* data, size_t length);
GLuint generateElementBuffer(const void* data, size_t length);

#ifdef __cplusplus
}
#endif
