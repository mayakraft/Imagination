#version 330

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

in vec3 position;
in vec3 normal;
out vec3 fragPosition;
out vec3 worldNormal;

void main() {
	// Calculate the vertex position in world space
	fragPosition = vec3(u_model * vec4(position, 1.0));

	// Calculate the normal in world space
	worldNormal = mat3(transpose(inverse(u_model))) * normal;

	// Calculate the vertex position in clip space
	gl_Position = u_projection * u_view * vec4(fragPosition, 1.0);
}

// #version 140

// uniform mat4 u_projection;
// uniform mat4 u_modelView;
// in vec3 position;
// in vec3 normal;
// out vec3 v_position;
// out vec3 v_normal;

// void main() {
// 	v_normal = normal;
// 	vec4 worldPosition = u_projection * u_modelView * vec4(position, 1.0);
// 	v_position = worldPosition.xyz;
// 	gl_Position = worldPosition;
// }
