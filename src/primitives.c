#include "./primitives.h"

// struct Tetrahedron: Polyhedron {
// 	Tetrahedron() {
// 		vertices = new float[4 * 3] {
// 			1.0, 0.0, 0.0,
// 			-0.3333, -0.9428, 0.0,
// 			-0.3333, 0.4714, 0.81649,
// 			-0.3333, 0.4714, -0.8164,
// 		};
// 		edges = short[6 * 2] {
// 			2, 3, 2, 0, 2, 1, 3, 0, 3, 1, 0, 1
// 		};
// 		faces = new short[4 * 3] {
// 			2, 1, 3,
// 			2, 3, 0,
// 			2, 0, 1,
// 			3, 1, 0,
// 		};
// 	}
// };

// int _sphere_stacks = 20; // 7;
// int _sphere_slices = 30; // 13;
// static float _unit_circle_outline_vertices[192];
// static float _unit_circle_outline_normals[192];
// static float _unit_circle_outline_texCoord[192];
// static float _unit_circle_fill_vertices[198]; // includes 1 more point: the center
// static float _unit_circle_fill_normals[198];
// static float _unit_circle_fill_texCoord[198];
// float *_unit_sphere_vertices, *_unit_sphere_normals, *_unit_sphere_texture;

// void initPrimitives(){
// 	static unsigned char _geometry_initialized = 0;
// 	if (!_geometry_initialized) {
// 		// CIRCLE
// 		for(int i = 0; i < 64; i++){
// 			_unit_circle_outline_vertices[i*3+0] = -sinf(M_PI*2/64.0f*i);
// 			_unit_circle_outline_vertices[i*3+1] = cosf(M_PI*2/64.0f*i);
// 			_unit_circle_outline_vertices[i*3+2] = 0.0f;
// 			_unit_circle_outline_normals[i*3+0] = _unit_circle_outline_normals[i*3+1] = 0.0;
// 			_unit_circle_outline_normals[i*3+2] = 1.0;
// 			_unit_circle_outline_texCoord[i*3+0] = -sinf(M_PI*2/64.0f*i)*0.5 + 0.5;
// 			_unit_circle_outline_texCoord[i*3+1] = cosf(M_PI*2/64.0f*i)*0.5 + 0.5;
// 		}
// 		_unit_circle_fill_vertices[0] = 0.0f;
// 		_unit_circle_fill_vertices[1] = 0.0f;
// 		_unit_circle_fill_vertices[2] = 0.0f;
// 		_unit_circle_fill_normals[0] = _unit_circle_fill_normals[1] = 0.0f;
// 		_unit_circle_fill_normals[2] = 1.0f;
// 		_unit_circle_fill_texCoord[0] = 0.0f;
// 		_unit_circle_fill_texCoord[1] = 0.0f;
// 		for(int i = 1; i <= 65; i++){
// 			_unit_circle_fill_vertices[i*3+0] = -sinf(M_PI*2/64.0f*(i-1));
// 			_unit_circle_fill_vertices[i*3+1] = cosf(M_PI*2/64.0f*(i-1));
// 			_unit_circle_fill_vertices[i*3+2] = 0.0f;
// 			_unit_circle_fill_normals[i*3+0] = _unit_circle_fill_normals[i*3+1] = 0.0f;
// 			_unit_circle_fill_normals[i*3+2] = 1.0f;
// 			_unit_circle_fill_texCoord[i*3+0] = -sinf(M_PI*2/64.0f*(i-1))*0.5 + 0.5f;
// 			_unit_circle_fill_texCoord[i*3+1] = cosf(M_PI*2/64.0f*(i-1))*0.5 + 0.5f;
// 		}
// 		// SPHERE
// 		// GLfloat m_Scale = 1;
// 		// GLfloat *vPtr = _unit_sphere_vertices = (GLfloat*)malloc(sizeof(GLfloat) * 3 * ((_sphere_slices*2+2) * (_sphere_stacks)));
// 		// GLfloat *nPtr = _unit_sphere_normals = (GLfloat*)malloc(sizeof(GLfloat) * 3 * ((_sphere_slices*2+2) * (_sphere_stacks)));
// 		// GLfloat *tPtr = _unit_sphere_texture = (GLfloat*)malloc(sizeof(GLfloat) * 2 * ((_sphere_slices*2+2) * (_sphere_stacks)));
// 		float m_Scale = 1;
// 		float *vPtr = _unit_sphere_vertices = (float*)malloc(sizeof(float) * 3 * ((_sphere_slices*2+2) * (_sphere_stacks)));
// 		float *nPtr = _unit_sphere_normals = (float*)malloc(sizeof(float) * 3 * ((_sphere_slices*2+2) * (_sphere_stacks)));
// 		float *tPtr = _unit_sphere_texture = (float*)malloc(sizeof(float) * 2 * ((_sphere_slices*2+2) * (_sphere_stacks)));
// 		for(unsigned int phiIdx = 0; phiIdx < _sphere_stacks; phiIdx++){
// 			// Latitude
// 			//starts at -pi/2 goes to pi/2
// 			float phi0 = M_PI * ((float)(phiIdx+0) * (1.0/(float)(_sphere_stacks)) - 0.5);  // the first circle
// 			float phi1 = M_PI * ((float)(phiIdx+1) * (1.0/(float)(_sphere_stacks)) - 0.5);  // second one
// 			float cosPhi0 = cos(phi0);
// 			float sinPhi0 = sin(phi0);
// 			float cosPhi1 = cos(phi1);
// 			float sinPhi1 = sin(phi1);
// 			for(unsigned int thetaIdx = 0; thetaIdx < _sphere_slices; thetaIdx++){
// 				//longitude
// 				float theta = 2.0*M_PI * ((float)thetaIdx) * (1.0/(float)(_sphere_slices - 1));
// 				float cosTheta = cos(theta+M_PI*.5);
// 				float sinTheta = sin(theta+M_PI*.5);
// 				vPtr[0] = m_Scale*cosPhi0 * cosTheta;
// 				vPtr[1] = m_Scale*(cosPhi0 * sinTheta);
// 				vPtr[2] = -m_Scale*sinPhi0;
// 				vPtr[3] = m_Scale*cosPhi1 * cosTheta;
// 				vPtr[4] = m_Scale*(cosPhi1 * sinTheta);
// 				vPtr[5] = -m_Scale*sinPhi1;
// 				nPtr[0] = cosPhi0 * cosTheta;
// 				nPtr[1] = cosPhi0 * sinTheta;
// 				nPtr[2] = -sinPhi0;
// 				nPtr[3] = cosPhi1 * cosTheta;
// 				nPtr[4] = cosPhi1 * sinTheta;
// 				nPtr[5] = -sinPhi1;
// 				// GLfloat texX = (float)thetaIdx * (1.0f/(float)(_sphere_slices-1));
// 				float texX = (float)thetaIdx * (1.0f/(float)(_sphere_slices-1));
// 				tPtr[0] = texX;
// 				tPtr[1] = (float)(phiIdx + 0) * (1.0f/(float)(_sphere_stacks));
// 				tPtr[2] = texX;
// 				tPtr[3] = (float)(phiIdx + 1) * (1.0f/(float)(_sphere_stacks));
// 				vPtr += 2*3;
// 				nPtr += 2*3;
// 				tPtr += 2*2;
// 			}
// 		}
// 		_geometry_initialized = 1;
// 	}
// }

#endif
