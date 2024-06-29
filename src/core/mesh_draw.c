#include "./mesh_draw.h"

void drawMeshFaces(mesh_t *poly) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, poly->vertices);
	glNormalPointer(GL_FLOAT, 0, poly->normals);
	glTexCoordPointer(2, GL_FLOAT, 0, poly->texCoords);
	glDrawElements(GL_TRIANGLES, poly->numFaces * 3, glindex_t, poly->faces);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void drawMeshLines(mesh_t *poly) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, poly->vertices);
	glNormalPointer(GL_FLOAT, 0, poly->vertices);
	glDrawElements(GL_LINES, poly->numEdges * 2, glindex_t, poly->edges);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void drawMeshPoints(mesh_t *poly) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, poly->vertices);
	glNormalPointer(GL_FLOAT, 0, poly->vertices);
	glDrawArrays(GL_POINTS, 0, poly->numVertices);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void drawMeshStripArray(mesh_t *poly) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, poly->vertices);
	glNormalPointer(GL_FLOAT, 0, poly->normals);
	glTexCoordPointer(2, GL_FLOAT, 0, poly->texCoords);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, poly->numFaces);
	// (_sphere_slices+1) * 2 * (_sphere_stacks-1)+2;
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}
