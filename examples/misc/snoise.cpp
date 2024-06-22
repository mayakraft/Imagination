#include <math.h>
#include "../glm/glm.hpp"

// glsl 2D simplex noise, Ian McEwan, Ashima Arts.
// this is the same simplex noise that is used in the shader
// https://stegu.github.io/webgl-noise/webdemo/

glm::vec2 floor(glm::vec2 v) {
	return glm::vec2(floor(v[0]), floor(v[1]));
}
glm::vec3 floor(glm::vec3 v) {
	return glm::vec3(floor(v[0]), floor(v[1]), floor(v[2]));
}
glm::vec4 floor(glm::vec4 v) {
	return glm::vec4(floor(v[0]), floor(v[1]), floor(v[2]), floor(v[3]));
}
glm::vec2 mod289(glm::vec2 x) {
	return x - floor(x * (1.0f / 289.0f)) * 289.0f;
}
glm::vec3 mod289(glm::vec3 x) {
	return x - floor(x * (1.0f / 289.0f)) * 289.0f;
}
glm::vec4 mod289(glm::vec4 x) {
	return x - floor(x * (1.0f / 289.0f)) * 289.0f;
}
glm::vec3 permute(glm::vec3 x) {
	return mod289(((x * 34.0f) + 10.0f) * x);
}
float snoise(glm::vec2 v) {
	const glm::vec4 C = glm::vec4(0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);
	glm::vec2 i = floor(v + dot(v, glm::vec2(C.y, C.y)));
	glm::vec2 x0 = v - i + dot(i, glm::vec2(C.x, C.x));
	glm::vec2 i1;
	i1 = (x0.x > x0.y) ? glm::vec2(1.0, 0.0) : glm::vec2(0.0, 1.0);
	glm::vec4 x12 = glm::vec4(x0.x, x0.y, x0.x, x0.y) + glm::vec4(C.x, C.x, C.z, C.z);
	x12 = glm::vec4(x12[0] - i1.x, x12[1] - i1.y, x12[2], x12[3]);
	i = mod289(i);
	glm::vec3 p = permute( permute( i.y + glm::vec3(0.0, i1.y, 1.0 )) + i.x + glm::vec3(0.0, i1.x, 1.0 ));
	glm::vec2 x12xy = glm::vec2(x12.x, x12.y);
	glm::vec2 x12zw = glm::vec2(x12.z, x12.w);
	glm::vec3 m = max(
		0.5f - glm::vec3(dot(x0,x0), dot(x12xy, x12xy), dot(x12zw, x12zw)),
		0.0f);
	m = m * m;
	m = m * m;
	glm::vec3 x = 2.0f * fract(p * glm::vec3(C.w, C.w, C.w)) - 1.0f;
	glm::vec3 h = abs(x) - 0.5f;
	glm::vec3 ox = floor(x + 0.5f);
	glm::vec3 a0 = x - ox;
	m *= 1.79284291400159f - 0.85373472095314f * (a0 * a0 + h * h);
	glm::vec3 g;
	g.x = a0.x * x0.x + h.x * x0.y;
	g.y = a0.y * x12.x + h.y * x12.y;
	g.z = a0.z * x12.z + h.z * x12.w;
	return 130.0 * dot(m, g);
}
