#include "./mesh.h"
#include "../math/vector.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
// #include <stdio.h>

float* makeFacesNormal(mesh_t *poly) {
	float* normals = (float*)malloc(sizeof(float) * poly->numFaces * 3);
	for (uindex_t f = 0; f < poly->numFaces; f++) {
		// uindex_t f0 = poly->faces[f * 3 + 0];
		// uindex_t f1 = poly->faces[f * 3 + 1];
		// uindex_t f2 = poly->faces[f * 3 + 2];
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

	for (uindex_t f = 0; f < poly->numFaces; f++) {
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
	for (uindex_t v = 0; v < poly->numVertices; v++) {
		normalize3(&normals[v * 3]);
	}
	return normals;
};

float* makeRegularPolyhedraTexCoords(mesh_t *poly) {
	float* texCoords = (float*)malloc(sizeof(float) * poly->numVertices * 2);
	for (uindex_t v = 0; v < poly->numVertices; v++) {
		float vec[3];
		memcpy(vec, &poly->vertices[v * 3], sizeof(float) * 3);
		normalize3(vec);
		// theta, between 0 and pi
		float lat = acos(vec[2]);
		// phi, between -pi/2 and + pi/2
		float lon = atan2(vec[0], vec[1]);
		texCoords[v * 2 + 0] = lon / (2 * M_PI) + 0.5;
		texCoords[v * 2 + 1] = lat / M_PI;
		// having trouble with the wrap around for a spherical mesh.
		// everything is good until the point where it needs to
		// go from like 0.9 to 0.1 (it goes down, instead of wrapping around + to 0)
		// printf("%d: %f, %f\n", v, texCoords[v * 2 + 0], texCoords[v * 2 + 1]);
	}
	return texCoords;
}
