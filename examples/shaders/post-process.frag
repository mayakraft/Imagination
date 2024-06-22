#version 330 core

uniform sampler2D screenTexture;

out vec4 FragColor;
in vec2 TexCoords;

void main() {
	vec3 color = texture(screenTexture, TexCoords).rgb;
	// invert colors and add horizontal banding
	color = vec3(1.0) - color;
	if (int(gl_FragCoord.y / 4) % 2 == 0) color = vec3(0, 0, 0);
	FragColor = vec4(color, 1.0);
}
