#version 140

uniform float u_time;

in vec3 v_position;
out vec4 outColor;

void main() {
	float altitude = v_position.z * 0.5 + 0.5;
	vec3 c_grass = vec3(0.3, 0.7, 0.1) * altitude;
	vec3 c_snow = vec3(1.0, 1.0, 1.0);
	vec3 c_water = vec3(0.1, 0.3, 1.0) * (altitude + 0.3);
	vec3 water = c_water * (1 - smoothstep(0.1, 0.3, altitude));
	vec3 grass = c_grass * smoothstep(0.1, 0.3, altitude);
	vec3 snow = c_snow * smoothstep(0.8, 0.9, altitude);
	vec3 sum = water + grass + snow;
	outColor = vec4(clamp(sum.x, 0.0, 1.0), clamp(sum.y, 0.0, 1.0), clamp(sum.z, 0.0, 1.0), 1.0);
}
