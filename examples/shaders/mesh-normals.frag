#version 330 core

in vec3 v_normal;
in vec3 v_fragPos;

out vec4 FragColor;

void main() {
	vec3 norm = normalize(v_normal);
	// from [-1, 1] to [0, 1]
	vec3 color = norm * 0.5 + 0.5;
	FragColor = vec4(color, 1.0);
}
