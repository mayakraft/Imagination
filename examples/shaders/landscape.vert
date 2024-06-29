#version 140

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform vec2 u_offset;
in vec3 position;
out vec3 v_position;
out vec3 v_normal;

// glsl 2D simplex noise, Ian McEwan, Ashima Arts.
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+10.0)*x); }
float snoise(vec2 v) {
	const vec4 C = vec4(0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);
	vec2 i  = floor(v + dot(v, C.yy));
	vec2 x0 = v - i + dot(i, C.xx);
	vec2 i1;
	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;
	i = mod289(i);
	vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 )) + i.x + vec3(0.0, i1.x, 1.0 ));
	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
	m = m * m;
	m = m * m;
	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;
	m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);
	vec3 g;
	g.x = a0.x * x0.x + h.x * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}

float altitude(vec2 coord) {
	float nscale = 0.005;
	float hscale = 40;
	float noiseValue = 0
		+ snoise(coord * nscale * 1) * 1
		+ snoise(coord * nscale * 2) * 0.5
		+ snoise(coord * nscale * 4) * 0.25
		+ snoise(coord * nscale * 8) * 0.125
		+ snoise(coord * nscale * 16) * 0.0625
		+ snoise(coord * nscale * 32) * 0.03125
		+ snoise(coord * nscale * 64) * 0.015625
		+ snoise(coord * nscale * 128) * 0.0078125;
	return noiseValue * hscale;
}

void main() {
	float value = altitude(u_offset + position.xy);
	vec3 newPos = vec3(position.xy, value);
	vec3 clampedNewPos = vec3(position.xy, max(value, 0));

	vec2 up = vec2(0, 1);
	vec2 side = vec2(1, 0);
	vec2 down = vec2(0, -1);
	float upZ = altitude(u_offset + position.xy + up);
	float sideZ = altitude(u_offset + position.xy + side);
	float downZ = altitude(u_offset + position.xy + down);
	vec3 a = vec3(up, upZ * 0.5) - vec3(side, sideZ * 0.5);
	vec3 b = vec3(down, downZ * 0.5) - vec3(side, sideZ * 0.5);
	vec3 normal = normalize(cross(a, b));

	v_normal = normal;
	v_position = newPos;
	gl_Position = u_projection * u_modelView * vec4(clampedNewPos, 1.0);
}
