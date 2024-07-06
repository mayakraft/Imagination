#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec2 v_texCoord;
out vec3 v_position;

void main() {
	v_position = vec3(u_model * vec4(aPosition, 1.0));
	v_texCoord = aTexCoord;
	gl_Position = u_projection * u_view * vec4(v_position, 1.0);
}
