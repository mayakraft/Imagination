#version 330 core

#define MAX_LIGHTS 10

struct Light {
	vec3 position;
	vec3 color;
};

uniform Light u_lights[MAX_LIGHTS];
uniform int u_numLights;
uniform vec3 u_ambientColor;
uniform vec3 u_materialColor;

in vec3 v_normal;
in vec3 v_fragPos;

out vec4 outColor;

void main() {
	// Calculate the normal per face using derivatives
	vec3 dFdx = dFdx(v_fragPos);
	vec3 dFdy = dFdy(v_fragPos);
	vec3 normal = normalize(cross(dFdx, dFdy));

	vec3 ambient = u_ambientColor * u_materialColor;
	vec3 result = ambient;

	for (int i = 0; i < u_numLights; ++i) {
		vec3 lightDir = normalize(u_lights[i].position - v_fragPos);
		float diff = max(dot(normal, lightDir), 0.0);
		// vec3 diffuse = diff * u_lights[i].color * 0.5; // not correct, but looks nice
		vec3 diffuse = diff * u_lights[i].color * u_materialColor;
		result += diffuse;
	}

	outColor = vec4(result, 1.0);
}
