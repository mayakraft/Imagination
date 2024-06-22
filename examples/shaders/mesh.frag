#version 330

uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;

in vec3 fragPosition;
in vec3 worldNormal;
out vec4 outColor;

void main() {
	// Ambient lighting
	float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * u_lightColor;

	// Diffuse lighting
	vec3 norm = normalize(worldNormal);
	vec3 lightDir = normalize(u_lightPos - fragPosition);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * u_lightColor;

	// Combine the lighting components
	vec3 result = (ambient + diffuse) * u_objectColor;
	outColor = vec4(result, 1.0);
}

// #version 140

// uniform vec3 u_color;
// in vec3 v_position;
// in vec3 v_normal;
// out vec4 outColor;

// void main() {
// 	float z = v_position.z;
// 	float distance = clamp((-z+4)*0.75, 0, 1);
// 	vec3 color = u_color * distance;
// 	outColor = vec4(color, 1.0);
// }
