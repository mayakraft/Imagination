#version 450

// dvec3 type would takes two slots (location n and n+1)
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
	gl_Position = vec4(inPosition, 1.0);
	fragColor = inColor;
}
