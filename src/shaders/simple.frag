#version 140

uniform float u_time;

out vec4 outColor;

void main() {
	outColor = vec4(
		sin(u_time * 1.05 + 0.8) * 0.5 + 0.5,
		sin(u_time * 0.87 + 2.1) * 0.5 + 0.5,
		sin(u_time * 0.72 + 1.2) * 0.5 + 0.5,
		1.0);
}
