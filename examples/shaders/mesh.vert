#version 330 core

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 v_fragPos;
out vec3 v_normal;

void main() {
	v_normal = mat3(transpose(inverse(u_model))) * normal;
	v_fragPos = vec3(u_model * vec4(position, 1.0));
	gl_Position = u_projection * u_view * vec4(v_fragPos, 1.0);
}
