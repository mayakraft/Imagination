#version 330 core

const int MAX_LIGHTS = 10;

struct Light {
	vec3 position;
	vec3 color;
};

uniform sampler2D u_texture;
uniform sampler2D u_heightmap;
uniform Light u_lights[MAX_LIGHTS];
uniform int u_numLights;
uniform vec3 u_ambientColor;

in vec2 v_texCoord;
in vec3 v_position;

out vec4 outColor;

vec3 calculateNormalFromHeightmap() {
	vec2 texelSize = 1.0 / textureSize(u_heightmap, 0);
	float height = texture(u_heightmap, v_texCoord).s;
	float heightLeft = texture(u_heightmap, v_texCoord + vec2(-texelSize.x, 0.0)).s;
	float heightRight = texture(u_heightmap, v_texCoord + vec2(texelSize.x, 0.0)).s;
	float heightUp = texture(u_heightmap, v_texCoord + vec2(0.0, texelSize.y)).s;
	float heightDown = texture(u_heightmap, v_texCoord + vec2(0.0, -texelSize.y)).s;

	return normalize(vec3(heightLeft - heightRight, heightDown - heightUp, 0.1));
}

void main() {
	vec3 normal = calculateNormalFromHeightmap();

  vec3 lighting = u_ambientColor;
	for (int i = 0; i < u_numLights; ++i) {
		vec3 lightDir = normalize(u_lights[i].position - v_position);
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * u_lights[i].color;
		lighting += diffuse;
	}

	vec3 textureColor = texture(u_texture, v_texCoord).rgb;
	vec3 result = lighting * textureColor;
	outColor = vec4(result, 1.0);
}
