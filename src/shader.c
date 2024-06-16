#include "shader.h"

// make sure to free the returned value after you are done using it
char *readFile(const char *filename) {
	char *buffer = 0;
	long length;
	FILE* f;
#ifdef _WIN32
	fopen_s(&f, filename, "rb");
#else
	f = fopen(filename, "rb");
#endif
	if (f == NULL) {
		fputs("shader file does not exist", stderr);
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = (char*)malloc(length + 1);
	if(buffer) fread(buffer, 1, length, f);
	fclose(f);
	buffer[length] = 0; // fixes occasional extra characters at end of buffer
	return buffer;
}

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
	if (glIsProgram(program)) {
		// Program log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;
		// Get info string length
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		char infoLog[maxLength];
		// Get info log
		glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0) {
			//Print Log
			printf( "%s\n", infoLog );
		}
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
		char infoLog[maxLength];
		// Get info log
		glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
		if (infoLogLength > 0) {
			// Print Log
			printf( "%s\n", infoLog );
		}
	} else {
		printf( "Name %d is not a shader\n", shader );
	}
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

ShaderProgram createProgram(
	const GLchar* vertexShaderSource,
	const GLchar* fragmentShaderSource) {
	GLuint gVBO = 0;
	GLuint gIBO = 0;

	// https://discourse.libsdl.org/t/lazy-foo-sdl-and-modern-opengl-tutorial-vao/23020
	GLuint vaoId = 0;
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);
	// Generate program
	GLuint gProgramID = glCreateProgram();
	// create and attach vertex shader to program
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	glAttachShader(gProgramID, vertexShader);
	// create and attach fragment shader to program
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	glAttachShader(gProgramID, fragmentShader);

	// Link program
	glLinkProgram(gProgramID);
	// Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(gProgramID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE) {
		printProgramLog(gProgramID);
		fputs("Error linking program", stderr);
	}

	// vertex attribute location
	GLint gPositionAttribute = glGetAttribLocation(gProgramID, "pos");
	if (gPositionAttribute == -1) {
		fputs("invalid attrib location", stderr);
	}

	// Initialize clear color
	// glClearColor(0.1f, 0.3f, 0.6f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// VBO data
	float scale = 1.0f; // 0.85f;
	GLfloat vertexData[] = {
		-scale, -scale,
		scale, -scale,
		scale, scale,
		-scale, scale
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

	ShaderProgram shader;
	shader.programID = gProgramID;
	shader.positionAttribute = gPositionAttribute;
	shader.vbo = gVBO;
	shader.ibo = gIBO;
	shader.vertexShader = vertexShader;
	shader.fragmentShader = fragmentShader;
	return shader;
}

void deallocProgram(ShaderProgram *program) {
	// these two can happen earlier
	glDetachShader(program->programID, program->vertexShader);
	glDetachShader(program->programID, program->fragmentShader);
	glDeleteProgram(program->programID);
}
