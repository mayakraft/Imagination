#version 330

uniform sampler2D tex;

uniform float u_width;
uniform float u_height;

uniform mat4 u_projection;
uniform mat4 u_modelView;
// uniform vec2 u_offset;
in vec3 position;
out vec3 v_position;
out vec3 v_normal;

void main() {
	// gl_VertexIndex;
	// gl_VertexID;
	// int x = gl_VertexID % int(u_width);
	// int y = int(floor(gl_VertexID / u_width));
	// vec2 texCoord = vec2(x / u_width, y / u_height);
	vec2 texCoord = vec2(position.x / u_width, position.y / u_height);
	vec4 color = texture(tex, texCoord);
	float z = color.z * 60;
	vec3 pos3D = vec3(position.xy, z);
	v_normal = vec3(0, 0, 1);
	v_position = pos3D;
	gl_Position = u_projection * u_modelView * vec4(pos3D, 1.0);
}
