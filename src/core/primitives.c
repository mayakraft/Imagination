#include "./primitives.h"
#include <stdio.h>

void makeTetrahedron(mesh_t *polyhedron) {
	const uindex_t numV = 4;
	const uindex_t numE = 6;
	const uindex_t numF = 4;
	float v[] = {
		1.0, 0.0, 0.0,
		-0.3333, -0.9428, 0.0,
		-0.3333, 0.4714, 0.81649,
		-0.3333, 0.4714, -0.8164,
	};
	uindex_t e[] = {
		2, 3, 2, 0, 2, 1, 3, 0, 3, 1, 0, 1,
	};
	uindex_t f[] = {
		2, 1, 3,
		2, 3, 0,
		2, 0, 1,
		3, 1, 0,
	};
	polyhedron->numVertices = numV;
	polyhedron->numEdges = numE;
	polyhedron->numFaces = numF;
	polyhedron->vertices = (float*)malloc(sizeof(float) * numV * 3);
	polyhedron->edges = (uindex_t*)malloc(sizeof(uindex_t) * numE * 2);
	polyhedron->faces = (uindex_t*)malloc(sizeof(uindex_t) * numF * 3);
	memcpy(polyhedron->vertices, v, sizeof(float) * numV * 3);
	memcpy(polyhedron->edges, e, sizeof(uindex_t) * numE * 2);
	memcpy(polyhedron->faces, f, sizeof(uindex_t) * numF * 3);
	polyhedron->normals = makeVerticesNormal(polyhedron);
	polyhedron->texCoords = makeRegularPolyhedraTexCoords(polyhedron);
};

void makeTetrahedronDual(mesh_t *polyhedron) {
	const uindex_t numV = 4;
	const uindex_t numE = 6;
	const uindex_t numF = 4;
	float v[] = {
		-1.0, 0.0, 0.0,
		0.3333, 0.9428, 0.0,
		0.3333, -0.4714, 0.81649,
		0.3333, -0.4714, -0.81649,
	};
	uindex_t e[] = {
		2, 3, 2, 0, 2, 1, 3, 0, 3, 1, 0, 1,
	};
	uindex_t f[] = {
		2, 1, 3,
		2, 3, 0,
		2, 0, 1,
		3, 1, 0,
	};
	polyhedron->numVertices = numV;
	polyhedron->numEdges = numE;
	polyhedron->numFaces = numF;
	polyhedron->vertices = (float*)malloc(sizeof(float) * numV * 3);
	polyhedron->edges = (uindex_t*)malloc(sizeof(uindex_t) * numE * 2);
	polyhedron->faces = (uindex_t*)malloc(sizeof(uindex_t) * numF * 3);
	memcpy(polyhedron->vertices, v, sizeof(float) * numV * 3);
	memcpy(polyhedron->edges, e, sizeof(uindex_t) * numE * 2);
	memcpy(polyhedron->faces, f, sizeof(uindex_t) * numF * 3);
	polyhedron->normals = makeVerticesNormal(polyhedron);
	polyhedron->texCoords = makeRegularPolyhedraTexCoords(polyhedron);
};

void makeOctahedron(mesh_t *polyhedron) {
	const uindex_t numV = 6;
	const uindex_t numE = 12;
	const uindex_t numF = 8;
	float v[6 * 3] = {
		1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, -1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0
	};
	uindex_t e[12 * 2] = {
		1, 0, 1, 2, 1, 5, 1, 3, 3, 2, 2, 0, 0, 5, 5, 3, 4, 2, 4, 3, 4, 5, 4, 0
	};
	uindex_t f[8 * 3] = {
		1, 0, 2, 1, 5, 0, 4, 0, 5, 4, 2, 0, 1, 2, 3, 1, 3, 5, 4, 5, 3, 4, 3, 2
	};
	polyhedron->numVertices = numV;
	polyhedron->numEdges = numE;
	polyhedron->numFaces = numF;
	polyhedron->vertices = (float*)malloc(sizeof(float) * numV * 3);
	polyhedron->edges = (uindex_t*)malloc(sizeof(uindex_t) * numE * 2);
	polyhedron->faces = (uindex_t*)malloc(sizeof(uindex_t) * numF * 3);
	memcpy(polyhedron->vertices, v, sizeof(float) * numV * 3);
	memcpy(polyhedron->edges, e, sizeof(uindex_t) * numE * 2);
	memcpy(polyhedron->faces, f, sizeof(uindex_t) * numF * 3);
	polyhedron->normals = makeVerticesNormal(polyhedron);
	polyhedron->texCoords = makeRegularPolyhedraTexCoords(polyhedron);
};

// 12 triangle faces
void makeCube(mesh_t *polyhedron) {
	const uindex_t numV = 8;
	const uindex_t numE = 12;
	const uindex_t numF = 12;
	float v[8 * 3] = {
		0.57735, 0.57735, 0.57735, 0.57735, 0.57735, -0.57735, 0.57735, -0.57735, -0.57735, 0.57735, -0.57735, 0.57735, -0.57735, 0.57735, 0.57735, -0.57735, 0.57735, -0.57735, -0.57735, -0.57735, -0.57735, -0.57735, -0.57735, 0.57735
	};
	uindex_t e[12 * 2] = {
		0, 1, 1, 2, 2, 3, 3, 0, 0, 4, 1, 5, 2, 6, 3, 7, 4, 5, 5, 6, 6, 7, 7, 4
	};
	uindex_t f[12 * 3] = {
		0, 2, 3, 2, 0, 1, 4, 1, 0, 1, 4, 5, 0, 3, 7, 7, 4, 0, 7, 5, 4, 5, 7, 6, 3, 6, 7, 6, 3, 2, 1, 5, 6, 6, 2, 1
	};
	polyhedron->numVertices = numV;
	polyhedron->numEdges = numE;
	polyhedron->numFaces = numF;
	polyhedron->vertices = (float*)malloc(sizeof(float) * numV * 3);
	polyhedron->edges = (uindex_t*)malloc(sizeof(uindex_t) * numE * 2);
	polyhedron->faces = (uindex_t*)malloc(sizeof(uindex_t) * numF * 3);
	memcpy(polyhedron->vertices, v, sizeof(float) * numV * 3);
	memcpy(polyhedron->edges, e, sizeof(uindex_t) * numE * 2);
	memcpy(polyhedron->faces, f, sizeof(uindex_t) * numF * 3);
	polyhedron->normals = makeVerticesNormal(polyhedron);
	polyhedron->texCoords = makeRegularPolyhedraTexCoords(polyhedron);
};

void makeIcosahedron(mesh_t *polyhedron) {
	const uindex_t numV = 12;
	const uindex_t numE = 30;
	const uindex_t numF = 20;
	float v[12 * 3] = {
		0.44721, -0.2763, 0.85065, -0.44721, 0.2763, 0.85065, -0.44721, 0.2763, -0.85065, 0.44721, -0.2763, -0.85065, -0.44721, -0.7236, 0.52573, 0.44721, 0.7236, 0.52573, 0.44721, 0.7236, -0.52573, -0.44721, -0.7236, -0.52573, 0.44721, -0.8944, 0.0, 1.0, 0.0, 0.0, -0.44721, 0.8944, 0.0, -1.0, 0.0, 0.0
	};
	uindex_t e[30 * 2] = {
		0, 8, 0, 9, 0, 1, 0, 4, 0, 5, 8, 3, 8, 9, 8, 7, 8, 4, 9, 3, 9, 6, 9, 5, 7, 4, 7, 3, 7, 2, 7, 11, 2, 10, 2, 11, 2, 3, 2, 6, 10, 11, 10, 5, 10, 6, 10, 1, 11, 1, 11, 4, 4, 1, 5, 1, 5, 6, 6, 3
	};
	uindex_t f[20 * 3] = {
		8, 7, 4, 7, 8, 3, 8, 4, 0, 8, 0, 9, 9, 3, 8, 9, 0, 5, 9, 5, 6, 9, 6, 3, 3, 2, 7, 3, 6, 2, 0, 4, 1, 0, 1, 5, 11, 4, 7, 11, 7, 2, 11, 2, 10, 1, 11, 10, 11, 1, 4, 10, 6, 5, 10, 5, 1, 10, 2, 6
	};
	polyhedron->numVertices = numV;
	polyhedron->numEdges = numE;
	polyhedron->numFaces = numF;
	polyhedron->vertices = (float*)malloc(sizeof(float) * numV * 3);
	polyhedron->edges = (uindex_t*)malloc(sizeof(uindex_t) * numE * 2);
	polyhedron->faces = (uindex_t*)malloc(sizeof(uindex_t) * numF * 3);
	memcpy(polyhedron->vertices, v, sizeof(float) * numV * 3);
	memcpy(polyhedron->edges, e, sizeof(uindex_t) * numE * 2);
	memcpy(polyhedron->faces, f, sizeof(uindex_t) * numF * 3);
	polyhedron->normals = makeVerticesNormal(polyhedron);
	polyhedron->texCoords = makeRegularPolyhedraTexCoords(polyhedron);
};

// 36 triangle faces
void makeDodecahedron(mesh_t *polyhedron) {
	const uindex_t numV = 20;
	const uindex_t numE = 30;
	const uindex_t numF = 36;
	float v[20 * 3] = {
		-0.7946, 0.4911, 0.3568, -0.1875, 0.7946, -0.5773, -0.7946, 0.4911, -0.3568, -0.7946, -0.1875, -0.5773, 0.1875, 0.3035, -0.9341, 0.1875, 0.9822, 0.0, 0.1875, -0.7946, -0.5773, -0.1875, -0.3035, -0.9341, -0.1875, -0.9822, 0.0, -0.7946, -0.6070, 0.0, 0.7946, -0.4911, -0.3568, 0.1875, -0.7946, 0.5773, -0.1875, -0.3035, 0.9341, -0.7946, -0.1875, 0.5773, 0.7946, -0.4911, 0.3568, -0.1875, 0.7946, 0.5773, 0.1875, 0.3035, 0.9341, 0.7946, 0.1875, 0.5773, 0.7946, 0.6070, 0.0, 0.7946, 0.1875, -0.5773
	};
	uindex_t e[30 * 2] = {
		19, 18, 18, 17, 17, 14, 14, 10, 10, 19, 14, 11, 11, 8, 8, 6, 6, 10, 19, 4, 4, 1, 1, 5, 5, 18, 9, 3, 3, 7, 7, 6, 8, 9, 13, 9, 3, 2, 2, 0, 0, 13, 12, 11, 12, 13, 1, 2, 5, 15, 15, 0, 15, 16, 16, 17, 16, 12, 4, 7
	};
	uindex_t f[36 * 3] = {
		5, 2, 1, 15, 0, 5, 2, 5, 0, 9, 2, 13, 2, 0, 13, 3, 2, 9, 10, 17, 19, 18, 19, 17, 14, 17, 10, 19, 1, 4, 6, 9, 8, 1, 18, 5, 11, 14, 8, 18, 1, 19, 8, 9, 11, 10, 8, 14, 8, 10, 6, 6, 7, 9, 9, 7, 3, 13, 12, 9, 12, 11, 9, 18, 15, 5, 17, 15, 18, 15, 17, 16, 11, 12, 14, 14, 12, 17, 17, 12, 16, 12, 13, 16, 13, 0, 16, 0, 15, 16, 7, 2, 3, 2, 7, 1, 1, 7, 4, 4, 7, 19, 19, 7, 10, 6, 10, 7
	};
	polyhedron->numVertices = numV;
	polyhedron->numEdges = numE;
	polyhedron->numFaces = numF;
	polyhedron->vertices = (float*)malloc(sizeof(float) * numV * 3);
	polyhedron->edges = (uindex_t*)malloc(sizeof(uindex_t) * numE * 2);
	polyhedron->faces = (uindex_t*)malloc(sizeof(uindex_t) * numF * 3);
	memcpy(polyhedron->vertices, v, sizeof(float) * numV * 3);
	memcpy(polyhedron->edges, e, sizeof(uindex_t) * numE * 2);
	memcpy(polyhedron->faces, f, sizeof(uindex_t) * numF * 3);
	polyhedron->normals = makeVerticesNormal(polyhedron);
	polyhedron->texCoords = makeRegularPolyhedraTexCoords(polyhedron);
};

// width and height are the number of subdivisions in each direction,
// as well as the width and height in the XY plane.
// each quad is 1x1 unit.
void makeQuadPlane(mesh_t *plane, unsigned int width, unsigned int height) {
	const uindex_t numV = (width + 1) * (height + 1);
	const uindex_t numE = width * (height + 1) + (width + 1) * height;
	const uindex_t numF = width * height * 2;
	// todo: edges.

	plane->numVertices = numV;
	plane->numEdges = numE;
	plane->numFaces = numF;
	plane->vertices = (float*)malloc(sizeof(float) * numV * 3);
	plane->normals = (float*)malloc(sizeof(float) * numV * 3);
	plane->texCoords = (float*)malloc(sizeof(float) * numV * 2);
	plane->edges = (uindex_t*)malloc(sizeof(uindex_t) * numE * 2);
	plane->faces = (uindex_t*)malloc(sizeof(uindex_t) * numF * 3);

	for (unsigned int y = 0; y < (height + 1); y++) {
		for (unsigned int x = 0; x < (width + 1); x++) {
			int index = x + y * (width + 1);
			plane->vertices[index * 3 + 0] = x;
			plane->vertices[index * 3 + 1] = y;
			plane->vertices[index * 3 + 2] = 0;
			plane->normals[index * 3 + 0] = 0;
			plane->normals[index * 3 + 1] = 0;
			plane->normals[index * 3 + 2] = 1;
			plane->texCoords[index * 2 + 0] = (float)x / (width + 1);
			plane->texCoords[index * 2 + 1] = (float)y / (height + 1);
		}
	}

	for (unsigned int i = 0; i < numF * 3; i++) { plane->faces[i] = 0; }
	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			unsigned int index = x + y * width;
			unsigned int index2 = x + y * (width + 1);
			plane->faces[index * 6 + 0] = index2;
			plane->faces[index * 6 + 1] = index2 + width + 1;
			plane->faces[index * 6 + 2] = index2 + 1;
			plane->faces[index * 6 + 3] = index2 + width + 1;
			plane->faces[index * 6 + 4] = index2 + width + 2;
			plane->faces[index * 6 + 5] = index2 + 1;
		}
	}
}

void makeCircle(mesh_t *circle) {
	const int NUM_PTS = 64;
	const uindex_t numV = NUM_PTS;
	const uindex_t numE = NUM_PTS;
	const uindex_t numF = NUM_PTS - 2;
	// todo: edges.

	circle->numVertices = numV;
	circle->numEdges = numE;
	circle->numFaces = numF;
	circle->vertices = (float*)malloc(sizeof(float) * numV * 3);
	circle->normals = (float*)malloc(sizeof(float) * numV * 3);
	circle->texCoords = (float*)malloc(sizeof(float) * numV * 2);
	circle->edges = (uindex_t*)malloc(sizeof(uindex_t) * numE * 2);
	circle->faces = (uindex_t*)malloc(sizeof(uindex_t) * numF * 3);

	for (unsigned int i = 0; i < numV; i++) {
		circle->vertices[i * 3 + 0] = -sinf(((M_PI * 2.0f) / numV) * i);
		circle->vertices[i * 3 + 1] = cosf(((M_PI * 2.0f) / numV) * i);
		circle->vertices[i * 3 + 2] = 0.0f;
		circle->normals[i * 3 + 0] = 0.0f;
		circle->normals[i * 3 + 1] = 0.0f;
		circle->normals[i * 3 + 2] = 1.0f;
		circle->texCoords[i * 2 + 0] = -sinf(((M_PI * 2.0f) / numV) * i) * 0.5 + 0.5;
		circle->texCoords[i * 2 + 1] = cosf(((M_PI * 2.0f) / numV) * i) * 0.5 + 0.5;
	}
	for (unsigned int i = 0; i < numF; i++) {
		circle->faces[i * 3 + 0] = 0;
		circle->faces[i * 3 + 1] = i + 1;
		circle->faces[i * 3 + 2] = i + 2;
	}
}

void makeUVSphere(mesh_t *sphere, float radius) {
	int stacks = 20; // 7;
	int slices = 30; // 13;

	sphere->numVertices = ((slices*2+2) * (stacks));
	sphere->numEdges = 0;
	sphere->numFaces = slices * stacks * 2;

	sphere->vertices = (float*)malloc(sizeof(float) * sphere->numVertices * 3);
	sphere->normals = (float*)malloc(sizeof(float) * sphere->numVertices * 3);
	sphere->texCoords = (float*)malloc(sizeof(float) * sphere->numVertices * 2);
	sphere->edges = (uindex_t*)malloc(sizeof(uindex_t) * sphere->numEdges * 2);
	sphere->faces = (uindex_t*)malloc(sizeof(uindex_t) * sphere->numFaces * 3);

	float *vPtr = sphere->vertices;
	float *nPtr = sphere->normals;
	float *tPtr = sphere->texCoords;

	for (int phiIdx = 0; phiIdx < stacks; phiIdx++){
		// Latitude
		// starts at -pi/2 goes to pi/2
		float phi0 = M_PI * ((float)(phiIdx+0) * (1.0/(float)(stacks)) - 0.5);  // the first circle
		float phi1 = M_PI * ((float)(phiIdx+1) * (1.0/(float)(stacks)) - 0.5);  // second one
		float cosPhi0 = cos(phi0);
		float sinPhi0 = sin(phi0);
		float cosPhi1 = cos(phi1);
		float sinPhi1 = sin(phi1);
		for (int thetaIdx = 0; thetaIdx < slices; thetaIdx++){
			//longitude
			float theta = 2.0*M_PI * ((float)thetaIdx) * (1.0/(float)(slices - 1));
			float cosTheta = cos(theta + M_PI * 0.5);
			float sinTheta = sin(theta + M_PI * 0.5);
			vPtr[0] = radius*cosPhi0 * cosTheta;
			vPtr[1] = radius*(cosPhi0 * sinTheta);
			vPtr[2] = -radius*sinPhi0;
			vPtr[3] = radius*cosPhi1 * cosTheta;
			vPtr[4] = radius*(cosPhi1 * sinTheta);
			vPtr[5] = -radius*sinPhi1;
			nPtr[0] = cosPhi0 * cosTheta;
			nPtr[1] = cosPhi0 * sinTheta;
			nPtr[2] = -sinPhi0;
			nPtr[3] = cosPhi1 * cosTheta;
			nPtr[4] = cosPhi1 * sinTheta;
			nPtr[5] = -sinPhi1;
			float texX = (float)thetaIdx * (1.0f/(float)(slices-1));
			tPtr[0] = texX;
			tPtr[1] = (float)(phiIdx + 0) * (1.0f/(float)(stacks));
			tPtr[2] = texX;
			tPtr[3] = (float)(phiIdx + 1) * (1.0f/(float)(stacks));
			vPtr += 2*3;
			nPtr += 2*3;
			tPtr += 2*2;
		}
	}
}
