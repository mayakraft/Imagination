#version 140

uniform float u_time;
out vec3 v_color;

void main() {
	// build triangle vertices dynamically using gl_VertexID
	float r = 2 / 3.0;
	float angle = gl_VertexID * (6.28318530718 / 3.0) - u_time;
	gl_Position = vec4(r * cos(angle), r * sin(angle), 0.0, 1.0);
	v_color = vec3(
		(gl_VertexID + 0) % 3 == 0,
		(gl_VertexID + 1) % 3 == 0,
		(gl_VertexID + 2) % 3 == 0);
}
