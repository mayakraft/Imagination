#version 330 core

uniform sampler2D screenTexture;

out vec4 FragColor;
in vec2 TexCoords;

void main() {
	// Sobel kernels
	float kernelX[9] = float[9](
		-1, 0, 1,
		-2, 0, 2,
		-1, 0, 1
	);
	float kernelY[9] = float[9](
		-1, -2, -1,
		0, 0, 0,
		1, 2, 1
	);

	vec2 texOffset = 1.0 / textureSize(screenTexture, 0); // Size of a single texel
	vec2 offsets[9] = vec2[](
		vec2(-texOffset.x, texOffset.y),  // top-left
		vec2(0.0, texOffset.y),           // top-center
		vec2(texOffset.x, texOffset.y),   // top-right
		vec2(-texOffset.x, 0.0),          // center-left
		vec2(0.0, 0.0),                   // center-center
		vec2(texOffset.x, 0.0),           // center-right
		vec2(-texOffset.x, -texOffset.y), // bottom-left
		vec2(0.0, -texOffset.y),          // bottom-center
		vec2(texOffset.x, -texOffset.y)   // bottom-right
	);

	float edgeX = 0.0;
	float edgeY = 0.0;
	for (int i = 0; i < 9; i++) {
		vec3 texColor = texture(screenTexture, TexCoords + offsets[i]).rgb;
		float intensity = dot(texColor, vec3(0.299, 0.587, 0.114)); // Convert to grayscale
		edgeX += intensity * kernelX[i];
		edgeY += intensity * kernelY[i];
	}

	float edge = sqrt(edgeX * edgeX + edgeY * edgeY);
	vec3 edgeColor = vec3(1.0, 1.0, 1.0) * edge;
	vec3 color = texture(screenTexture, TexCoords).rgb;
	// vec3 finalColor = mix(color, edgeColor, clamp(edge * 10.0, 0.0, 1.0));
	vec3 finalColor = mix(color, edgeColor, edge);
	FragColor = vec4(finalColor, 1.0);
}

// void main() {
// 	vec3 color = texture(screenTexture, TexCoords).rgb;
// 	FragColor = vec4(color, 1.0);
// }

// void main() {
// 	vec3 color = texture(screenTexture, TexCoords).rgb;
// 	// invert colors and add horizontal banding
// 	color = vec3(1.0) - color;
// 	if (int(gl_FragCoord.y / 4) % 2 == 0) color = vec3(0, 0, 0);
// 	FragColor = vec4(color, 1.0);
// }
