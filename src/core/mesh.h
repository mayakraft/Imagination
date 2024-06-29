#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define uindex_t unsigned int
// #define uindex_t unsigned short
#define glindex_t GL_UNSIGNED_INT
// #define glindex_t GL_UNSIGNED_SHORT

typedef struct mesh_t {
	uindex_t numVertices;
	uindex_t numEdges;
	uindex_t numFaces;
	float* vertices;
	float* normals;
	float* texCoords;
	uindex_t* edges;
	uindex_t* faces;
} mesh_t;

float* makeFacesNormal(mesh_t *poly);
float* makeVerticesNormal(mesh_t *poly);

float* makeRegularPolyhedraTexCoords(mesh_t *poly);

#ifdef __cplusplus
}
#endif
