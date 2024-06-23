#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mesh_t {
	unsigned short numVertices;
	unsigned short numEdges;
	unsigned short numFaces;
	float* vertices;
	float* normals;
	float* texCoords;
	unsigned short* edges;
	unsigned short* faces;
} mesh_t;

float* makeFacesNormal(mesh_t *poly);
float* makeVerticesNormal(mesh_t *poly);

float* makeRegularPolyhedraTexCoords(mesh_t *poly);

#ifdef __cplusplus
}
#endif
