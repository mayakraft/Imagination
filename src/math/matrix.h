#pragma once

#ifdef __cplusplus
extern "C" {
#endif

unsigned char mat4Inverse(const float m[16], float inverse[16]);

// use this method to multiply matrices
void multiplyMatrices4(const float *a, const float *b, float *result);

// this method is limited, parameters cannot be equal, ie: (A, B, A)
void mat4x4MultUnique(const float *a, const float *b, float *result);

void makeMatrix4RotateX(float *m, float angle);
void makeMatrix4RotateY(float *m, float angle);
void makeMatrix4RotateZ(float *m, float angle);

void multiplyMatrix4Vector3(const float m[16], const float v[3], float result[3]);
void multiplyMatrix4Vector4(const float m[16], const float v[4], float result[4]);

void setIdentity4(float *m);

void transposeMatrix4(float *m);

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
