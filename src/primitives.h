#pragma once

#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct Polyhedron {
	float* vertices;
	short* edges;
	short* faces;
} Polyhedron;

void makeTetrahedron(Polyhedron *tetrahedron) {
	float v[] = {
		1.0, 0.0, 0.0,
		-0.3333, -0.9428, 0.0,
		-0.3333, 0.4714, 0.81649,
		-0.3333, 0.4714, -0.8164,
	};
	short e[] = {
		2, 3, 2, 0, 2, 1, 3, 0, 3, 1, 0, 1,
	};
	short f[] = {
		2, 1, 3,
		2, 3, 0,
		2, 0, 1,
		3, 1, 0,
	};
	tetrahedron->vertices = (float*)malloc(sizeof(float) * 3 * 4);
	tetrahedron->edges = (short*)malloc(sizeof(short) * 2 * 6);
	tetrahedron->faces = (short*)malloc(sizeof(short) * 3 * 4);
	memcpy(tetrahedron->vertices, v, sizeof(float) * 3 * 4);
	memcpy(tetrahedron->edges, e, sizeof(short) * 2 * 6);
	memcpy(tetrahedron->faces, f, sizeof(short) * 3 * 4);
};

void makeTetrahedronDual(Polyhedron *tetrahedron) {
	float v[] = {
		-1.0, 0.0, 0.0,
		0.3333, 0.9428, 0.0,
		0.3333, -0.4714, 0.81649,
		0.3333, -0.4714, -0.81649,
	};
	short e[] = {
		2, 3, 2, 0, 2, 1, 3, 0, 3, 1, 0, 1,
	};
	short f[] = {
		2, 1, 3,
		2, 3, 0,
		2, 0, 1,
		3, 1, 0,
	};
	tetrahedron->vertices = (float*)malloc(sizeof(float) * 3 * 4);
	tetrahedron->edges = (short*)malloc(sizeof(short) * 2 * 6);
	tetrahedron->faces = (short*)malloc(sizeof(short) * 3 * 4);
	memcpy(tetrahedron->vertices, v, sizeof(float) * 3 * 4);
	memcpy(tetrahedron->edges, e, sizeof(short) * 2 * 6);
	memcpy(tetrahedron->faces, f, sizeof(short) * 3 * 4);
};

void makeOctahedron(Polyhedron *octahedron) {
	float v[6 * 3] = {
		1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, -1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0
	};
	short e[12 * 2] = {
		1, 0, 1, 2, 1, 5, 1, 3, 3, 2, 2, 0, 0, 5, 5, 3, 4, 2, 4, 3, 4, 5, 4, 0
	};
	short f[8 * 3] = {
		1, 0, 2, 1, 5, 0, 4, 0, 5, 4, 2, 0, 1, 2, 3, 1, 3, 5, 4, 5, 3, 4, 3, 2
	};
	octahedron->vertices = (float*)malloc(sizeof(float) * 3 * 6);
	octahedron->edges = (short*)malloc(sizeof(short) * 2 * 12);
	octahedron->faces = (short*)malloc(sizeof(short) * 3 * 8);
	memcpy(octahedron->vertices, v, sizeof(float) * 3 * 6);
	memcpy(octahedron->edges, e, sizeof(short) * 2 * 12);
	memcpy(octahedron->faces, f, sizeof(short) * 3 * 8);
};

// 12 triangle faces
void makeCube(Polyhedron *cube) {
	float v[8 * 3] = {
		0.57735, 0.57735, 0.57735, 0.57735, 0.57735, -0.57735, 0.57735, -0.57735, -0.57735, 0.57735, -0.57735, 0.57735, -0.57735, 0.57735, 0.57735, -0.57735, 0.57735, -0.57735, -0.57735, -0.57735, -0.57735, -0.57735, -0.57735, 0.57735
	};
	short e[12 * 2] = {
		0, 1, 1, 2, 2, 3, 3, 0, 0, 4, 1, 5, 2, 6, 3, 7, 4, 5, 5, 6, 6, 7, 7, 4
	};
	short f[12 * 3] = {
		0, 2, 3, 2, 0, 1, 4, 1, 0, 1, 4, 5, 0, 3, 7, 7, 4, 0, 7, 5, 4, 5, 7, 6, 3, 6, 7, 6, 3, 2, 1, 5, 6, 6, 2, 1
	};
	cube->vertices = (float*)malloc(sizeof(float) * 3 * 8);
	cube->edges = (short*)malloc(sizeof(short) * 2 * 12);
	cube->faces = (short*)malloc(sizeof(short) * 3 * 12);
	memcpy(cube->vertices, v, sizeof(float) * 3 * 8);
	memcpy(cube->edges, e, sizeof(short) * 2 * 12);
	memcpy(cube->faces, f, sizeof(short) * 3 * 12);
};

void makeIcosahedron(Polyhedron *icosahedron) {
	float v[12 * 3] = {
		0.44721, -0.2763, 0.85065, -0.44721, 0.2763, 0.85065, -0.44721, 0.2763, -0.85065, 0.44721, -0.2763, -0.85065, -0.44721, -0.7236, 0.52573, 0.44721, 0.7236, 0.52573, 0.44721, 0.7236, -0.52573, -0.44721, -0.7236, -0.52573, 0.44721, -0.8944, 0.0, 1.0, 0.0, 0.0, -0.44721, 0.8944, 0.0, -1.0, 0.0, 0.0
	};
	short e[30 * 2] = {
		0, 8, 0, 9, 0, 1, 0, 4, 0, 5, 8, 3, 8, 9, 8, 7, 8, 4, 9, 3, 9, 6, 9, 5, 7, 4, 7, 3, 7, 2, 7, 11, 2, 10, 2, 11, 2, 3, 2, 6, 10, 11, 10, 5, 10, 6, 10, 1, 11, 1, 11, 4, 4, 1, 5, 1, 5, 6, 6, 3
	};
	short f[20 * 3] = {
		8, 7, 4, 7, 8, 3, 8, 4, 0, 8, 0, 9, 9, 3, 8, 9, 0, 5, 9, 5, 6, 9, 6, 3, 3, 2, 7, 3, 6, 2, 0, 4, 1, 0, 1, 5, 11, 4, 7, 11, 7, 2, 11, 2, 10, 1, 11, 10, 11, 1, 4, 10, 6, 5, 10, 5, 1, 10, 2, 6
	};
	icosahedron->vertices = (float*)malloc(sizeof(float) * 3 * 12);
	icosahedron->edges = (short*)malloc(sizeof(short) * 2 * 30);
	icosahedron->faces = (short*)malloc(sizeof(short) * 3 * 20);
	memcpy(icosahedron->vertices, v, sizeof(float) * 3 * 12);
	memcpy(icosahedron->edges, e, sizeof(short) * 2 * 30);
	memcpy(icosahedron->faces, f, sizeof(short) * 3 * 20);
};

// 36 triangle faces
void makeDodecahedron(Polyhedron *dodecahedron) {
	float v[20 * 3] = {
		-0.7946, 0.4911, 0.3568, -0.1875, 0.7946, -0.5773, -0.7946, 0.4911, -0.3568, -0.7946, -0.1875, -0.5773, 0.1875, 0.3035, -0.9341, 0.1875, 0.9822, 0.0, 0.1875, -0.7946, -0.5773, -0.1875, -0.3035, -0.9341, -0.1875, -0.9822, 0.0, -0.7946, -0.6070, 0.0, 0.7946, -0.4911, -0.3568, 0.1875, -0.7946, 0.5773, -0.1875, -0.3035, 0.9341, -0.7946, -0.1875, 0.5773, 0.7946, -0.4911, 0.3568, -0.1875, 0.7946, 0.5773, 0.1875, 0.3035, 0.9341, 0.7946, 0.1875, 0.5773, 0.7946, 0.6070, 0.0, 0.7946, 0.1875, -0.5773
	};
	short e[30 * 2] = {
		19, 18, 18, 17, 17, 14, 14, 10, 10, 19, 14, 11, 11, 8, 8, 6, 6, 10, 19, 4, 4, 1, 1, 5, 5, 18, 9, 3, 3, 7, 7, 6, 8, 9, 13, 9, 3, 2, 2, 0, 0, 13, 12, 11, 12, 13, 1, 2, 5, 15, 15, 0, 15, 16, 16, 17, 16, 12, 4, 7
	};
	short f[36 * 3] = {
		5, 2, 1, 15, 0, 5, 2, 5, 0, 9, 2, 13, 2, 0, 13, 3, 2, 9, 10, 17, 19, 18, 19, 17, 14, 17, 10, 19, 1, 4, 6, 9, 8, 1, 18, 5, 11, 14, 8, 18, 1, 19, 8, 9, 11, 10, 8, 14, 8, 10, 6, 6, 7, 9, 9, 7, 3, 13, 12, 9, 12, 11, 9, 18, 15, 5, 17, 15, 18, 15, 17, 16, 11, 12, 14, 14, 12, 17, 17, 12, 16, 12, 13, 16, 13, 0, 16, 0, 15, 16, 7, 2, 3, 2, 7, 1, 1, 7, 4, 4, 7, 19, 19, 7, 10, 6, 10, 7
	};
	dodecahedron->vertices = (float*)malloc(sizeof(float) * 3 * 20);
	dodecahedron->edges = (short*)malloc(sizeof(short) * 2 * 30);
	dodecahedron->faces = (short*)malloc(sizeof(short) * 3 * 36);
	memcpy(dodecahedron->vertices, v, sizeof(float) * 3 * 20);
	memcpy(dodecahedron->edges, e, sizeof(short) * 2 * 30);
	memcpy(dodecahedron->faces, f, sizeof(short) * 3 * 36);
};
