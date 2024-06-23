#include <time.h>
#include "../include/engine.h"

// unsigned char* getTextureData(const char * filename, int width, int height){
// 	FILE * file;
// 	file = fopen(filename, "rb");
// 	if (file == NULL) return 0;
// 	unsigned char *data = (unsigned char *)malloc(width * height * 3);
// 	// unsigned char *data = (unsigned char *)malloc(width * height * 4);
// 	fread(data, width * height * 3, 1, file);
// 	// fread(data, width * height * 4, 1, file);
// 	fclose(file);
// 	for(int i = 0; i < width * height; i++){
// 		int index = i*3;
// 		unsigned char B,R;
// 		B = data[index];
// 		R = data[index+2];
// 		data[index] = R;
// 		data[index+2] = B;
// 	}
// 	return data;
// }

// GLuint loadTextureFileBGR(const char * filename, int width, int height){
// 	GLuint texture;
// 	unsigned char *data = getTextureData(filename, width, height);
// 	glGenTextures(1, &texture);
// 	glBindTexture(GL_TEXTURE_2D, texture);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// 	// gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR, GL_UNSIGNED_BYTE, data);
// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
// 	glBindTexture(GL_TEXTURE_2D, 0);
// 	return texture;
// }

// GLuint loadTextureFile(const char * filename, int width, int height){
// 	GLuint texture;
// 	unsigned char *data = getTextureData(filename, width, height);
// 	glGenTextures(1, &texture);
// 	glBindTexture(GL_TEXTURE_2D, texture);
// 	// glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// 	// gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
// 	// gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
// 	glBindTexture(GL_TEXTURE_2D, 0);
// 	return texture;
// }

// GLuint loadTextureFileSmooth(const char * filename, int width, int height){
// 	GLuint texture;
// 	unsigned char * data = getTextureData(filename, width, height);
// 	glGenTextures(1, &texture);
// 	glBindTexture(GL_TEXTURE_2D, texture);
// 	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// 	// gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
// 	glBindTexture(GL_TEXTURE_2D, 0);
// 	return texture;
// }

GLuint loadTexture(unsigned char *data, int width, int height){
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// yes?
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 1);
	// glTexParameteri(GL_TEXTURE_BASE_LEVEL | GL_TEXTURE_MAX_LEVEL | GL_TEXTURE_MIN_LOD | GL_TEXTURE_MAX_LOD);
	// no?
	// glad_glGenerateMipmap
	// glad_glGenerateTextureMipmap
	// glad_glGenerateTextureMipmapEXT
	// glad_glGenerateMipmapEXT
	// glad_glGenerateMultiTexMipmapEXT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	// gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGRA, GL_UNSIGNED_BYTE, data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

// GLuint loadTexture(void* pixels, int width, int height){
// 	GLuint texture;
// 	glGenTextures(1, &texture);
// 	glBindTexture(GL_TEXTURE_2D, texture);
// 	// glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
// 	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

// 	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
// 	// GL_RGBA
// 	glBindTexture(GL_TEXTURE_2D, 0);
// 	return texture;
// }

int main() {
	srand(time(NULL));
	int SCREEN = 640;
	int frame = 0;

	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "GL Texture Test",
		.width = SCREEN,
		.height = SCREEN,
		.useLegacy = true,
	};
	GameEngine engine = initGLEngine(params);

	SDL_Delay(200);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// SDL_Surface* wallSurface = IMG_Load("examples/images/wall1024.png");
	SDL_Surface* wallSurface = IMG_Load("examples/images/wall.png");
	SDL_Surface* rgb24 = SDL_ConvertSurfaceFormat(
		wallSurface,
		SDL_PIXELFORMAT_RGB24,
		0);
	unsigned char *pixels = (unsigned char*)rgb24->pixels;
	GLuint wallTexture = loadTexture(pixels, rgb24->w, rgb24->h);
	printf("surface (original) %d,%d (x%d)\n", wallSurface->w, wallSurface->h, wallSurface->format->BytesPerPixel);
	printf("surface (converted) %d,%d (x%d)\n", rgb24->w, rgb24->h, rgb24->format->BytesPerPixel);
	printf("texture id %d\n", wallTexture);

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat perspective[16];
	makePerspectiveMatrix4(45, 1.0 / 1.0, 0.1f, 100.0f, perspective);
	glLoadMatrixf(perspective);

	// model view matrix
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	GLfloat lookat[16];
	makeLookAtMatrix4(0, -1, 2, 0, 0, 0, 0, 0, 1, lookat);
	glLoadMatrixf(lookat);

	float vertices[] = {
		0, 0, 0,
		1, 0, 0,
		1, 1, 0,
		1, 1, 0,
		0, 1, 0,
		0, 0, 0,
	};
	float texCoords[] = {
		0, 0,
		1, 0,
		1, 1,
		1, 1,
		0, 1,
		0, 0,
	};

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
				}
			}
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		glPushMatrix();
		glRotatef(frame / 4.0, 0, 0, 1);

		glPushMatrix();
		glTranslatef(-0.5, -0.5, 0);

		glBindTexture(GL_TEXTURE_2D, wallTexture);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
		glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glBindTexture(GL_TEXTURE_2D, 0);

		glPopMatrix(); // translate
		glPopMatrix(); // rotate

		SDL_GL_SwapWindow(engine.window);

		frame += 1;
	}

	// SDL_FreeSurface(wall);
	dealloc(&engine);
	return 0;
}
