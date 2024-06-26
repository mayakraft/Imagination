#pragma once

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "./mesh.h"

#ifdef __cplusplus
extern "C" {
#endif

void makeTetrahedron(mesh_t *tetrahedron);
void makeTetrahedronDual(mesh_t *tetrahedron);
void makeOctahedron(mesh_t *octahedron);
void makeCube(mesh_t *cube);
void makeIcosahedron(mesh_t *icosahedron);
void makeDodecahedron(mesh_t *dodecahedron);
void makeQuadPlane(mesh_t *plane, unsigned int width, unsigned int height);
void makeCircle(mesh_t *circle);
void makeUVSphere(mesh_t *sphere, float radius);

#ifdef __cplusplus
}
#endif
