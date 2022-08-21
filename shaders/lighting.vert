#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coords;

out VS_OUT {
	vec3 pos;
	vec3 normal;
	vec2 tex_coords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_out.pos = vec3(model * vec4(a_pos, 1.0));
	vs_out.normal = mat3(transpose(inverse(model))) * a_normal;

	vs_out.tex_coords = a_tex_coords;

	gl_Position = projection * view * model * vec4(a_pos.xyz, 1.0f);
}
