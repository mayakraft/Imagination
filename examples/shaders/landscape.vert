#version 140

uniform mat4 u_projection;
uniform mat4 u_modelView;

in vec3 position;
out vec3 v_position;

void main() {
	v_position = position;
	gl_Position = u_projection * u_modelView * vec4(position, 1.0);
}
