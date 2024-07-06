#version 330 core

in vec3 aPosition;
in vec2 aTexCoord;

uniform sampler2D u_texture;
uniform sampler2D u_heightmap;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform bool u_doDisplace;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_texCoord;
out vec3 v_color;

vec3 calculateNormalFromHeightmap(vec2 coord, float scaleFactor) {
	vec2 texelSize = 1.0 / textureSize(u_heightmap, 0);
	float heightCenter = texture(u_heightmap, coord).s;
	float heightLeft = texture(u_heightmap, coord + vec2(-texelSize.x, 0.0)).s;
	float heightRight = texture(u_heightmap, coord + vec2(texelSize.x, 0.0)).s;
	float heightBottom = texture(u_heightmap, coord + vec2(0.0, -texelSize.y)).s;
	float heightTop = texture(u_heightmap, coord + vec2(0.0, texelSize.y)).s;
	float dX = heightLeft - heightRight;
	float dY = heightBottom - heightTop;
	return normalize(vec3(dX, dY, 1.0 / scaleFactor));
}

void main() {
	// the magnitude of the height map may need some adjusting
	float depthScale = 5;
	float height = texture(u_heightmap, aTexCoord).s;
	vec3 pos3D = float(!u_doDisplace) * aPosition
		+ float(u_doDisplace) * vec3(aPosition.xy, height * depthScale);

	v_position = pos3D;
	v_normal = calculateNormalFromHeightmap(aTexCoord, depthScale);
	v_texCoord = aTexCoord;
	v_color = texture(u_texture, aTexCoord).rgb;

	gl_Position = u_projection * u_view * u_model * vec4(pos3D, 1.0);
}
