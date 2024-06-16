#version 140

// shader by Kishimisu
// https://www.youtube.com/watch?v=f4s1h2YETNY

uniform vec2 u_resolution;
uniform float u_time;

out vec4 outColor;

vec3 palette(float t) {
	vec3 a = vec3(0.5);
	vec3 b = vec3(0.5);
	vec3 c = vec3(1.0);
	vec3 d = vec3(0.263, 0.416, 0.557);
	return a + b * cos(6.28318 * (c * t + d));
}

vec4 kaleidoscope(vec2 uv, float time, float aspect) {
	uv = (uv - 0.5) * 3.0;
	uv.y *= aspect;
	vec2 uv0 = uv;
	vec3 finalColor = vec3(0.0);
	for (float i = 0.0; i < 4.0; i++) {
		uv = fract(uv * 1.5) - 0.5;
		float d = length(uv) * exp(-length(uv0));
		vec3 col = palette(length(uv0) + i * 0.4 + time * 0.4);
		d = sin(d * 8.0 + time)/8.;
		d = abs(d);
		d = pow(0.01 / d, 1.2);
		finalColor += col * d;
	}
	return vec4(finalColor, 1.0);
}

void main() {
	float aspect = u_resolution.x / u_resolution.y;
	vec2 uv = vec2(gl_FragCoord.x, gl_FragCoord.y) / u_resolution;
	outColor = kaleidoscope(uv, u_time, aspect);
}
