#version 140

uniform float u_time;
uniform mat4 u_projection;
uniform mat4 u_modelView;

in vec3 v_position;
in vec3 v_normal;
out vec4 outColor;

void main() {
	float altitude = (v_position.z / 40) * 0.5 + 0.5;

	vec3 c_grass = vec3(0.3, 0.7, 0.1) * altitude;
	vec3 c_snow = vec3(1.0, 1.0, 1.0);
	vec3 c_water = vec3(0.1, 0.3, 1.0) * (altitude + 0.3);
	vec3 water = c_water * (1 - smoothstep(0.1, 0.3, altitude));
	vec3 grass = c_grass * smoothstep(0.1, 0.3, altitude);
	float snowlevel = smoothstep(0.8, 1.0, altitude);
	vec3 sum = mix(water + grass, c_snow, snowlevel);

	vec3 texture = vec3(clamp(sum.x, 0.0, 1.0), clamp(sum.y, 0.0, 1.0), clamp(sum.z, 0.0, 1.0));

	// mat4 world = u_projection * u_modelView;
	// mat4 invWorld = inverse(world);
	// vec3 forward = normalize((invWorld * vec4(0.0, 0.0, 1.0, 1.0)).xyz);

	vec3 sun = normalize(vec3(cos(u_time), sin(u_time), 1.0));
	float lightvec = dot(v_normal, sun);

	vec3 north = normalize(vec3(0, 1, 1));
	vec3 east = normalize(vec3(1, 0, 1));
	vec3 south = normalize(vec3(0, -1, 1));
	vec3 west = normalize(vec3(-1, 0, 1));
	float northLight = dot(v_normal, north);
	float eastLight = dot(v_normal, east);
	float southLight = dot(v_normal, south);
	float westLight = dot(v_normal, west);

	// vec3 baseColor = mix(texture * 0.4, texture, smoothstep(0.4, 1.0, lightvec));
	vec3 color1 = mix(texture * 0.2, texture, smoothstep(0.4, 1.0, eastLight));
	vec3 color2 = mix(texture * 0.2, texture, smoothstep(0.4, 1.0, westLight));
	vec3 shine = mix(texture * 0.1, vec3(1.0, 1.0, 1.0), smoothstep(0.98, 1.0, lightvec));

	outColor = vec4(color1 + color2 + shine, 1.0);
}
