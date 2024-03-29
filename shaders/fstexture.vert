#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_coord;

out vec2 o_tex_coord;

void main()
{
	gl_Position = vec4(a_pos, 1.0);
	o_tex_coord = a_tex_coord;
}
