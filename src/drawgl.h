// this is old OpenGL 2.0 syntax style

#include "SDL2/SDL_opengl.h"
#include "./primitives.h"

void drawPolyhedronFaces(Polyhedron *poly, int numFaces){
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, poly->vertices);
	glNormalPointer(GL_FLOAT, 0, poly->vertices);
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_SHORT, poly->faces);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void drawPolyhedronLines(Polyhedron *poly, int numEdges){
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, poly->vertices);
	glNormalPointer(GL_FLOAT, 0, poly->vertices);
	glDrawElements(GL_LINES, numEdges * 2, GL_UNSIGNED_SHORT, poly->edges);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void drawPolyhedronPoints(Polyhedron *poly, int numVertices){
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, poly->vertices);
	glNormalPointer(GL_FLOAT, 0, poly->vertices);
	glDrawArrays(GL_POINTS, 0, numVertices);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}
