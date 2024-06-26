#version 330

// uniform float u_time;
uniform float u_width;
uniform float u_height;
uniform mat4 u_projection;
uniform mat4 u_modelView;

in vec3 v_position;
in vec3 v_normal;
out vec4 outColor;

void main() {
	float altitude = v_position.z;
	// outColor = vec4(vec3(1.0, 1.0, 1.0), 1.0);
	outColor = vec4(vec3(
	v_position.x / u_width,
	v_position.y / u_height,
	v_position.z / 50),
	1.0);
}
