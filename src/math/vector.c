#include "vector.h"
#include <math.h>

float magnitude3(const float* v) {
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

// modifies input vector
void normalize3(float* vector) {
	float m = magnitude3(vector);
	if (m != 0) {
		vector[0] /= m;
		vector[1] /= m;
		vector[2] /= m;
	}
}

void cross3 (const float* v, const float* u, float* result) {
	result[0] = v[1] * u[2] - v[2] * u[1];
	result[1] = v[2] * u[0] - v[0] * u[2];
	result[2] = v[0] * u[1] - v[1] * u[0];
}

void subtract3(const float* v, const float* u, float* result) {
	result[0] = v[0] - u[0];
	result[1] = v[1] - u[1];
	result[2] = v[2] - u[2];
}
