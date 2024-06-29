#version 330 core

in vec3 v_fragPos;

out vec4 outColor;

void main() {
	// Calculate the normal per face using derivatives
	vec3 dFdx = dFdx(v_fragPos);
	vec3 dFdy = dFdy(v_fragPos);
	vec3 normal = normalize(cross(dFdx, dFdy));
	// from [-1, 1] to [0, 1]
	vec3 normalMapColor = normal * 0.5 + 0.5;
	outColor = vec4(normalMapColor, 1.0);
}
