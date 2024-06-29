#include "shader.h"

//GLuint loadShader(char *vertex_path, char *fragment_path) {
//	// GLenum err = glewInit();
//	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	char *vSource = readFile(vertex_path);
//	char *fSource = readFile(fragment_path);
//	GLint result = GL_FALSE;
//	int logLength;
//	// vertex
//	glShaderSource(vertexShader, 1, (const char *const *)&vSource, NULL);
//	glCompileShader(vertexShader);
//	// Check vertex shader
//	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
//	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
//	if(logLength){
//		char errorLog[logLength];
//		glGetShaderInfoLog(vertexShader, logLength, NULL, &errorLog[0]);
//		printf("VERTEX SHADER COMPILE %s", &errorLog[0]);
//	}
//	// fragment
//	glShaderSource(fragmentShader, 1, (const char *const *)&fSource, NULL);
//	glCompileShader(fragmentShader);
//	// Check fragment shader
//	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
//	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
//	if(logLength){
//		char errorLog[logLength];
//		glGetShaderInfoLog(fragmentShader, logLength, NULL, &errorLog[0]);
//		printf("FRAGMENT SHADER COMPILE %s", &errorLog[0]);
//	}
//	free(vSource);
//	free(fSource);
//	GLuint program = glCreateProgram();
//	glAttachShader(program, vertexShader);
//	glAttachShader(program, fragmentShader);
//	glLinkProgram(program);
//	glGetProgramiv(program, GL_LINK_STATUS, &result);
//	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
//	if (logLength) {
//		char programError[logLength]; programError[0] = 0;
//		glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
//		printf("LINKER %s", &programError[0]);
//	}
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//	return program;
//}

void printProgramLog(GLuint program) {
	// Make sure name is shader
	if (!glIsProgram(program)) {
		printf("Name %d is not a program\n", program);
		return;
	}
	int infoLogLength = 0;
	int maxLength = infoLogLength;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
	char *infoLog = (char*)malloc(sizeof(char) * maxLength);
	glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
	if (infoLogLength > 0) { printf("%s\n", infoLog); }
	free(infoLog);
}

void printShaderLog(GLuint shader) {
	// Make sure name is shader
	if (!glIsShader(shader)) {
		printf("Name %d is not a shader\n", shader);
		return;
	}
	int infoLogLength = 0;
	int maxLength = infoLogLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
	char *infoLog = (char*)malloc(sizeof(char) * maxLength);
	glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
	if (infoLogLength > 0) { printf("%s\n", infoLog); }
	free(infoLog);
}

// GL_FRAGMENT_SHADER 0x8B30, GL_VERTEX_SHADER 0x8B31
GLuint compileShader(unsigned short type, const GLchar* shaderSource) {
	// Create shader
	GLuint shader = glCreateShader(type);
	// Set source
	glShaderSource(shader, 1, &shaderSource, NULL);
	// Compile source
	glCompileShader(shader);
	// Check shader for errors
	GLint didCompile = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &didCompile);
	if (didCompile != GL_TRUE) {
		printf("Unable to compile shader (%d) %d!\n", type, shader);
		printShaderLog(shader);
		fputs("Unable to compile shader", stderr);
	}
	return shader;
}

GLuint createShaderProgram(const GLchar* vertex, const GLchar* fragment) {
	// Generate program
	GLuint program = glCreateProgram();
	// create and attach vertex shader to program
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertex);
	glAttachShader(program, vertexShader);
	// create and attach fragment shader to program
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragment);
	glAttachShader(program, fragmentShader);
	// Link program
	glLinkProgram(program);
	// Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(program, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE) {
		printProgramLog(program);
		fputs("Error linking program", stderr);
	}
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	// i don't think we should automatically use the program.
	// glUseProgram(program);
	return program;
}

GLint getAttrib(GLuint program, const char* name) {
	GLint attribute = glGetAttribLocation(program, name);
	if (attribute == -1) { fputs("invalid attrib name", stderr); }
	return attribute;
}

GLint getUniform(GLuint program, const char* name) {
	GLint uniform = glGetUniformLocation(program, name);
	if (uniform == -1) { fputs("invalid uniform name", stderr); }
	return uniform;
}

// length: for example, 2 * 4 * sizeof(GLfloat)
GLuint makeArrayBuffer(const void* data, size_t length) {
	GLuint bufferObject = 0;
	glGenBuffers(1, &bufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject);
	glBufferData(GL_ARRAY_BUFFER, length, data, GL_STATIC_DRAW);
	return bufferObject;
}

// length: for example, sizeof(GLuint) * 21 * 3
GLuint makeElementBuffer(const void* data, size_t length) {
	GLuint bufferObject = 0;
	glGenBuffers(1, &bufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, length, data, GL_STATIC_DRAW);
	return bufferObject;
}

void deallocProgram(GLuint program) {
	glDeleteProgram(program);
}

GLuint beginVAO() {
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	return vao;
}

void endVAO() {
	glBindVertexArray(0);
}
