#pragma once

#ifdef __cplusplus
extern "C" {
#endif

float magnitude3(const float* v);

// modifies input vector
void normalize3(float* vector);

void cross3 (const float* v, const float* u, float* result);

void subtract3(const float* v, const float* u, float* result);

#ifdef __cplusplus
}
#endif
