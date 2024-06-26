#version 330 core

const int MAX_LIGHTS = 10;

// position in view space
struct Light {
	vec3 position;
	vec3 color;
};

uniform sampler2D u_texture;
uniform sampler2D u_heightmap;
uniform Light u_lights[MAX_LIGHTS];
uniform int u_numLights;
// uniform mat4 u_projection;
// uniform mat4 u_modelView;

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texCoord;
in vec3 v_color;

out vec4 outColor;

void main() {
	// smooth, billinear
	// vec3 inColor = v_color;
	// not smooth, nearest neighbor
	vec3 inColor = texture(u_texture, v_texCoord).rgb;
	// outColor = inColor;

	vec3 normal = normalize(v_normal);
	vec3 finalColor = vec3(0.0);
	for (int i = 0; i < u_numLights; i++) {
		vec3 lightDir = normalize(u_lights[i].position - v_position);
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * u_lights[i].color * inColor;
		finalColor += diffuse;
	}
	outColor = vec4(finalColor, 1.0);
}
