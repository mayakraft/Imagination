#include "./mesh.h"
#include "../math/vector.h"
#include <stdlib.h>

float* makeFacesNormal(mesh_t *poly) {
	float* normals = (float*)malloc(sizeof(float) * poly->numFaces * 3);
	for (int f = 0; f < poly->numFaces; f++) {
		// unsigned short f0 = poly->faces[f * 3 + 0];
		// unsigned short f1 = poly->faces[f * 3 + 1];
		// unsigned short f2 = poly->faces[f * 3 + 2];
		float* p0 = &poly->vertices[poly->faces[f * 3 + 0] * 3];
		float* p1 = &poly->vertices[poly->faces[f * 3 + 1] * 3];
		float* p2 = &poly->vertices[poly->faces[f * 3 + 2] * 3];
		float a[3], b[3], res[3];
		subtract3(p1, p0, a);
		subtract3(p2, p0, b);
		cross3(a, b, res);
		normalize3(res);
		normals[f * 3 + 0] = res[0];
		normals[f * 3 + 1] = res[1];
		normals[f * 3 + 2] = res[2];
	}
	return normals;
}

float* makeVerticesNormal(mesh_t *poly) {
	float *faceNormals = makeFacesNormal(poly);

	// normals = (float*)malloc(sizeof(float) * poly->numVertices * 3);
	float* normals = (float*)calloc(poly->numVertices * 3, sizeof(float));

	for (int f = 0; f < poly->numFaces; f++) {
		// float x = faceNormals[f * 3 + 0];
		// float y = faceNormals[f * 3 + 1];
		// float z = faceNormals[f * 3 + 2];
		for (int i = 0; i < 3; i++) {
			int v = poly->faces[f * 3 + i];
			normals[v * 3 + 0] += faceNormals[f * 3 + 0];
			normals[v * 3 + 1] += faceNormals[f * 3 + 1];
			normals[v * 3 + 2] += faceNormals[f * 3 + 2];
		}
	}
	for (int v = 0; v < poly->numVertices; v++) {
		normalize3(&normals[v * 3]);
	}
	return normals;
};
