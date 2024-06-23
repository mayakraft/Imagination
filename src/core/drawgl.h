#pragma once

#ifdef _WIN32
#include "SDL_opengl.h"
#else
#include "SDL2/SDL_opengl.h"
#endif

#include "./primitives.h"

#ifdef __cplusplus
extern "C" {
#endif

// these are old OpenGL 2.0 syntax style
void drawMeshFaces(mesh_t *poly, int numFaces);
void drawMeshLines(mesh_t *poly, int numEdges);
void drawMeshPoints(mesh_t *poly, int numVertices);

#ifdef __cplusplus
}
#endif
