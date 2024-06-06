#include "primitives.h"
#include "SDL2/SDL_opengl.h"

void drawPolyhedronFaces(Polyhedron *poly, int numFaces){
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, poly->vertices);
	glNormalPointer(GL_FLOAT, 0, poly->vertices);
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_SHORT, poly->faces);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void drawPlatonicSolidLines(Polyhedron *poly, int numEdges){
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, poly->vertices);
	glNormalPointer(GL_FLOAT, 0, poly->vertices);
	glDrawElements(GL_LINES, numEdges * 2, GL_UNSIGNED_SHORT, poly->edges);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void drawPlatonicSolidPoints(Polyhedron *poly, int numVertices){
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, poly->vertices);
	glNormalPointer(GL_FLOAT, 0, poly->vertices);
	glDrawArrays(GL_POINTS, 0, numVertices);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

// void drawQuad(SDL_Renderer *renderer, SDL_Texture *texture) {
// 	SDL_Color color{ 0xff, 0xff, 0xff, 0xff };
// 		SDL_Vertex vertices[] = {
// 			{{ 0.f, 0.f }, color, { 0.0f, 0.0f }},
// 			{{ 1.f, 0.f }, color, { 1.0f, 0.0f }},
// 			{{ 1.f, 1.f }, color, { 0.0f, 1.0f }},
// 			{{ 0.f, 1.f }, color, { 1.0f, 1.0f }},
// 		};
// 		int indices[] = { 0, 1, 2, 2, 3, 1 };
// 		SDL_RenderGeometry(renderer, texture, vertices, 4, indices, 6);
// }

// void drawRendererTest(SDL_Renderer *renderer) {
// 	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
// 	SDL_RenderClear(renderer);
// 	// SDL_RenderFillRect(renderer, NULL);
// 	SDL_Rect rect1 = SDL_Rect { .x = 10, .y = 10, .w = 100, .h = 100 };
// 	SDL_Rect rect2 = SDL_Rect { .x = 110, .y = 10, .w = 100, .h = 100 };
// 	SDL_Rect rect3 = SDL_Rect { .x = 210, .y = 10, .w = 100, .h = 100 };
// 	SDL_SetRenderDrawColor(renderer, 0x11, 0x55, 0x88, 0xFF);
// 	SDL_RenderFillRect(renderer, &rect1);
// 	SDL_SetRenderDrawColor(renderer, 0xee, 0x55, 0x33, 0xFF);
// 	SDL_RenderFillRect(renderer, &rect2);
// 	SDL_SetRenderDrawColor(renderer, 0xff, 0xbb, 0x44, 0xFF);
// 	SDL_RenderFillRect(renderer, &rect3);
// }

// void drawPlatonicSolidFaces(char solidType){
// 	glEnableClientState(GL_VERTEX_ARRAY);
// 	glEnableClientState(GL_NORMAL_ARRAY);
// 	glVertexPointer(3, GL_FLOAT, 0, _platonic_point_arrays[solidType]);
// 	glNormalPointer(GL_FLOAT, 0, _platonic_point_arrays[ solidType ]);
// 	glDrawElements(GL_TRIANGLES, 3*_platonic_num_faces[solidType], GL_UNSIGNED_SHORT, _platonic_face_array[solidType]);
// 	glDisableClientState(GL_NORMAL_ARRAY);
// 	glDisableClientState(GL_VERTEX_ARRAY);
// }
// void drawPlatonicSolidLines(char solidType){
// 	glEnableClientState(GL_VERTEX_ARRAY);
// 	glEnableClientState(GL_NORMAL_ARRAY);
// 	glVertexPointer(3, GL_FLOAT, 0, _platonic_point_arrays[solidType]);
// 	glNormalPointer(GL_FLOAT, 0, _platonic_point_arrays[ solidType ]);
// 	glDrawElements(GL_LINES, 2*_platonic_num_lines[solidType], GL_UNSIGNED_SHORT, _platonic_line_array[solidType]);
// 	glDisableClientState(GL_NORMAL_ARRAY);
// 	glDisableClientState(GL_VERTEX_ARRAY);
// }
// void drawPlatonicSolidPoints(char solidType){
// 	glEnableClientState(GL_VERTEX_ARRAY);
// 	glEnableClientState(GL_NORMAL_ARRAY);
// 	glVertexPointer(3, GL_FLOAT, 0, _platonic_point_arrays[solidType]);
// 	glNormalPointer(GL_FLOAT, 0, _platonic_point_arrays[ solidType ]);
// 	glDrawArrays(GL_POINTS, 0, _platonic_num_vertices[solidType]);
// 	glDisableClientState(GL_NORMAL_ARRAY);
// 	glDisableClientState(GL_VERTEX_ARRAY);
// }

// void drawTetrahedron(SDL_Renderer *renderer, SDL_Texture *texture, float scale){
// 	SDL_Vertex vertices[] {
// 		{{ xy[0], xy[1] }, { 0xFF, 0x00, 0x00, 0xFF }, { 0.5f, 1.0f }},
// 		{{ xy[2], xy[3] }, { 0x00, 0xFF, 0x00, 0xFF }, { 0.0f, 0.0f }},
// 		{{ xy[4], xy[5] }, { 0x00, 0x00, 0xFF, 0xFF }, { 1.0f, 0.0f }},
// 	};
// 	int indices[] = { 0, 1, 2 };

// 	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
// 	SDL_RenderClear(renderer);
// 	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
// 	// _tetrahedron_points

// 	SDL_RenderGeometry(renderer, texture, vertices, 3, indices, 3);
// 	SDL_RenderPresent(renderer);

// 	switch(SHAPE_FILL){
// 		case 0: drawPlatonicSolidLines(0); break;
// 		default: drawPlatonicSolidFaces(0); break;
// 	}
// }
