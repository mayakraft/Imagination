#pragma once

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Polyhedron {
	float* vertices;
	unsigned short* edges;
	unsigned short* faces;
} Polyhedron;

void makeTetrahedron(Polyhedron *tetrahedron);
void makeTetrahedronDual(Polyhedron *tetrahedron);
void makeOctahedron(Polyhedron *octahedron);
// 12 triangle faces
void makeCube(Polyhedron *cube);
void makeIcosahedron(Polyhedron *icosahedron);
// 36 triangle faces
void makeDodecahedron(Polyhedron *dodecahedron);

#ifdef __cplusplus
}
#endif
