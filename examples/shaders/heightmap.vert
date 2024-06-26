#version 330 core

uniform sampler2D u_texture;
uniform sampler2D u_heightmap;
uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform bool u_doDisplace;

in vec3 position;
in vec2 texCoord;
out vec3 v_position;
out vec3 v_normal;
out vec2 v_texCoord;
out vec3 v_color;

vec3 calculateNormalFromHeightmap(vec2 coord, float scaleFactor) {
	vec2 texelSize = 1.0 / textureSize(u_heightmap, 0);
	vec2 offsetLeft = vec2(-texelSize.x, 0.0);
	vec2 offsetRight = vec2(texelSize.x, 0.0);
	vec2 offsetBottom = vec2(0.0, -texelSize.y);
	vec2 offsetTop = vec2(0.0, texelSize.y);

	float heightCenter = texture(u_heightmap, coord).r;
	float heightLeft = texture(u_heightmap, coord + offsetLeft).r;
	float heightRight = texture(u_heightmap, coord + offsetRight).r;
	float heightBottom = texture(u_heightmap, coord + offsetBottom).r;
	float heightTop = texture(u_heightmap, coord + offsetTop).r;

	float dX = heightRight - heightLeft;
	float dY = heightTop - heightBottom;
	return normalize(vec3(-dX, -dY, 1.0 / scaleFactor));
}

void main() {
	// the magnitude of the height map may need some adjusting
	float depthScale = 5;
	float height = texture(u_heightmap, texCoord).s;
	vec3 pos3D = float(!u_doDisplace) * position
		+ float(u_doDisplace) * vec3(position.xy, height * depthScale);

	v_position = pos3D;
	v_normal = calculateNormalFromHeightmap(texCoord, depthScale);
	v_texCoord = texCoord;
	v_color = texture(u_texture, texCoord).rgb;

	gl_Position = u_projection * u_modelView * vec4(pos3D, 1.0);
}
