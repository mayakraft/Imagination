#ifndef initialize_h
#define initialize_h

#include <GL/glew.h>
// #include <OpenGL/OpenGL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

struct ShaderProgram {
	GLuint programID;
	GLint positionAttribute;
	GLuint vbo;
	GLuint ibo;
};

char *readFile(const char *filename) {
	char *buffer = 0;
	long length;
	FILE* f;
#ifdef _WIN32
	fopen_s(&f, filename, "rb");
#else
	f = fopen(filename, "rb");
#endif
	if (f == NULL) { throw std::runtime_error("shader file does not exist"); }
	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = (char*)malloc(length + 1);
	if(buffer) fread(buffer, 1, length, f);
	fclose(f);
	buffer[length] = 0; // fixes occasional extra characters at end of buffer
	return buffer;
}

GLuint loadShader(char *vertex_path, char *fragment_path) {
	// GLenum err = glewInit();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char *vSource = readFile(vertex_path);
	char *fSource = readFile(fragment_path);
	GLint result = GL_FALSE;
	int logLength;
	// vertex
	glShaderSource(vertexShader, 1, (const char *const *)&vSource, NULL);
	glCompileShader(vertexShader);
	// Check vertex shader
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength){
		char errorLog[logLength];
		glGetShaderInfoLog(vertexShader, logLength, NULL, &errorLog[0]);
		printf("VERTEX SHADER COMPILE %s", &errorLog[0]);
	}
	// fragment
	glShaderSource(fragmentShader, 1, (const char *const *)&fSource, NULL);
	glCompileShader(fragmentShader);
	// Check fragment shader
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength){
		char errorLog[logLength];
		glGetShaderInfoLog(fragmentShader, logLength, NULL, &errorLog[0]);
		printf("FRAGMENT SHADER COMPILE %s", &errorLog[0]);
	}
	free(vSource);
	free(fSource);
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength) {
		char programError[logLength]; programError[0] = 0;
		glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
		printf("LINKER %s", &programError[0]);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return program;
}

void printProgramLog(GLuint program) {
	// Make sure name is shader
	if (glIsProgram(program)) {
		// Program log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;
		// Get info string length
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		// Allocate string
		char* infoLog = new char[ maxLength ];
		// Get info log
		glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0) {
			//Print Log
			printf( "%s\n", infoLog );
		}
		//Deallocate string
		delete[] infoLog;
	} else {
		printf( "Name %d is not a program\n", program );
	}
}

void printShaderLog(GLuint shader) {
	// Make sure name is shader
	if (glIsShader(shader)) {
		// Shader log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;
		// Get info string length
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
		// Allocate string
		char* infoLog = new char[ maxLength ];
		// Get info log
		glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			// Print Log
			printf( "%s\n", infoLog );
		}
		// Deallocate string
		delete[] infoLog;
	} else {
		printf( "Name %d is not a shader\n", shader );
	}
}

ShaderProgram createProgram(const char *vertex_path, const char *fragment_path) {
	GLuint gVBO = 0;
	GLuint gIBO = 0;
	// https://discourse.libsdl.org/t/lazy-foo-sdl-and-modern-opengl-tutorial-vao/23020
	GLuint vaoId = 0;
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);
	// Generate program
	GLuint gProgramID = glCreateProgram();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vertexShaderSource = readFile(vertex_path);
	// Set vertex source
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// Compile vertex source
	glCompileShader(vertexShader);
	// Check vertex shader for errors
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
	if (vShaderCompiled != GL_TRUE) {
		printShaderLog(vertexShader);
		throw std::runtime_error("Unable to compile vertex shader");
	}
	// Attach vertex shader to program
	glAttachShader(gProgramID, vertexShader);
	// Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Get fragment source
	const GLchar* fragmentShaderSource = readFile(fragment_path);
	// Set fragment source
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	// Compile fragment source
	glCompileShader(fragmentShader);
	// Check fragment shader for errors
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE) {
		printf("Unable to compile fragment shader %d!\n", fragmentShader);
		printShaderLog(fragmentShader);
		throw std::runtime_error("Unable to compile fragment shader");
	}
	// Attach fragment shader to program
	glAttachShader(gProgramID, fragmentShader);
	// Link program
	glLinkProgram(gProgramID);
	// Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(gProgramID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE) {
		printProgramLog(gProgramID);
		throw std::runtime_error("Error linking program");
	}

	// vertex attribute location
	GLint gPositionAttribute = glGetAttribLocation(gProgramID, "pos");
	if (gPositionAttribute == -1) {
		throw std::runtime_error("invalid attrib location");
	}

	// Initialize clear color
	glClearColor(0.1f, 0.3f, 0.6f, 1.0f);
	// VBO data
	GLfloat vertexData[] = {
		-0.85f, -0.85f,
		0.85f, -0.85f,
		0.85f, 0.85f,
		-0.85f, 0.85f
	};
	// IBO data
	GLuint indexData[] = { 0, 1, 2, 3 };
	// Create VBO
	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
	// Create IBO
	glGenBuffers(1, &gIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);

	return ShaderProgram {
		.programID = gProgramID,
		.positionAttribute = gPositionAttribute,
		.vbo = gVBO,
		.ibo = gIBO,
	};
}

#endif
