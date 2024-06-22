#pragma once

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Polyhedron {
	unsigned short numVertices;
	unsigned short numEdges;
	unsigned short numFaces;
	float* vertices;
	float* normals;
	unsigned short* edges;
	unsigned short* faces;
} Polyhedron;

void makeTetrahedron(Polyhedron *tetrahedron);
void makeTetrahedronDual(Polyhedron *tetrahedron);
void makeOctahedron(Polyhedron *octahedron);
void makeCube(Polyhedron *cube);
void makeIcosahedron(Polyhedron *icosahedron);
void makeDodecahedron(Polyhedron *dodecahedron);

#ifdef __cplusplus
}
#endif
