#include "texture.h"

GLuint loadGLTexture(
	unsigned char *data,
	int width,
	int height,
	GLuint inputFormat,
	GLuint storageFormat
) {
	GLuint texture;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// if mipmaps are not used, this is required
	// https://stackoverflow.com/a/13867751/1956418
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, storageFormat, width, height, 0, inputFormat, GL_UNSIGNED_BYTE, data);
	// glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

GLuint loadGLTextureFromFileRGB(const char* path) {
	SDL_Surface* surface = IMG_Load(path);
	SDL_Surface* surfaceConverted = SDL_ConvertSurfaceFormat(
		surface,
		SDL_PIXELFORMAT_RGB24,
		0);
	GLuint texture = loadGLTexture(
		(unsigned char*)surfaceConverted->pixels,
		surfaceConverted->w,
		surfaceConverted->h,
		GL_RGB,
		GL_RGB);
	SDL_FreeSurface(surface);
	SDL_FreeSurface(surfaceConverted);
	return texture;
}

GLuint loadGLTextureFromFileRGBA(const char* path) {
	SDL_Surface* surface = IMG_Load(path);
	SDL_Surface* surfaceConverted = SDL_ConvertSurfaceFormat(
		surface,
		SDL_PIXELFORMAT_RGBA32,
		0);
	GLuint texture = loadGLTexture(
		(unsigned char*)surfaceConverted->pixels,
		surfaceConverted->w,
		surfaceConverted->h,
		GL_RGBA,
		GL_RGBA);
	SDL_FreeSurface(surface);
	SDL_FreeSurface(surfaceConverted);
	return texture;
}

GLuint loadGLTextureFromFileGrayscale(const char* path) {
	SDL_Surface* surface = IMG_Load(path);
	SDL_Surface* surfaceConverted = SDL_ConvertSurfaceFormat(
		surface,
		SDL_PIXELFORMAT_RGB24,
		0);
	int width = surfaceConverted->w;
	int height = surfaceConverted->h;
	unsigned char* grayscaleData = (unsigned char*)malloc(width * height);
	unsigned char* pixels = (unsigned char*)surfaceConverted->pixels;
	for (int i = 0; i < width * height; i++) {
		unsigned char r = pixels[i * 3 + 0];
		unsigned char g = pixels[i * 3 + 1];
		unsigned char b = pixels[i * 3 + 2];
		grayscaleData[i] = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);
	}
	GLuint texture = loadGLTexture(
		grayscaleData,
		surfaceConverted->w,
		surfaceConverted->h,
		GL_RED,
		GL_RED);
	free(grayscaleData);
	SDL_FreeSurface(surface);
	SDL_FreeSurface(surfaceConverted);
	return texture;
}

GLuint updateGLTexture(
	GLuint texture,
	unsigned char *data,
	int width,
	int height,
	GLuint glImageFormat
) {
	glBindTexture(GL_TEXTURE_2D, texture);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// if mipmaps are not used, this is required
	// https://stackoverflow.com/questions/13867219/opengl-renders-texture-all-white
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, glImageFormat, width, height, 0, glImageFormat, GL_UNSIGNED_BYTE, data);
	// glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

SDL_Texture* loadSDLTexture(SDL_Renderer *renderer, const char* path) {
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
	} else {
		// Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		}
		// Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;
}
