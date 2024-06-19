#pragma once

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define D2R 0.01745329251994
#define R2D 57.295779513082321

#ifndef M_PI
#define M_PI 3.141592653589793238
#endif

#ifdef __cplusplus
extern "C" {
#endif

float modulusContext(float complete, int modulus);

float magnitude3(const float* v);

// modifies input vector
void normalize3(float* vector);

void cross3 (const float* v, const float* u, float* result);

void subtract3(const float* v, const float* u, float* result);

// MATRICES
unsigned char mat4Inverse(const float m[16], float inverse[16]);

void mat4x4MultUnique(const float *a, const float *b, float *result);

// use this method in all cases
void multiplyMatrices4(const float *a, const float *b, float *result);

void transposeMatrix4(float *m);

void makeMatrix4RotateX(float *m, float angle);

void makeMatrix4RotateY(float *m, float angle);

void makeMatrix4RotateZ(float *m, float angle);

void setIdentity4(float *m);

void multiplyMatrix4Vector3(const float m[16], const float v[3], float result[3]);

void multiplyMatrix4Vector4(const float m[16], const float v[4], float result[4]);

void makePerspectiveMatrix4(float FOV, float aspect, float near, float far, float* m);

void makeOrthographicMatrix4(
	float top, float right, float bottom, float left,
	float near, float far,
	float* m);

void makeLookAtMatrix4 (
	float posX, float posY, float posZ,
	float targetX, float targetY, float targetZ,
	float upX, float upY, float upZ,
	float* m);

void setMat4FromMat3(float *m4, float *m3);

#ifdef __cplusplus
}
#endif
