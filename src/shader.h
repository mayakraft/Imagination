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
	GLint positionAttribute;
	GLuint vbo;
	GLuint ibo;
	GLuint vertexShader;
	GLuint fragmentShader;
} ShaderProgram;

// make sure to free the returned value after you are done using it
char *readFile(const char *filename);

//GLuint loadShader(char *vertex_path, char *fragment_path);

void printProgramLog(GLuint program);

void printShaderLog(GLuint shader);

// GL_FRAGMENT_SHADER 0x8B30, GL_VERTEX_SHADER 0x8B31
GLuint compileShader(unsigned short type, const GLchar* shaderSource);

ShaderProgram createProgram(
	const GLchar* vertexShaderSource,
	const GLchar* fragmentShaderSource);

void deallocProgram(ShaderProgram *program);

#ifdef __cplusplus
}
#endif
