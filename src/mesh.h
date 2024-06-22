#pragma once

#include "./primitives.h"

#ifdef __cplusplus
extern "C" {
#endif

float* makeFacesNormal(Polyhedron *poly);
float* makeVerticesNormal(Polyhedron *poly);

#ifdef __cplusplus
}
#endif
