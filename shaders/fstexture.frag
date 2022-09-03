#version 330 core
out vec4 frag_color;

in vec2 o_tex_coord;

uniform sampler2D render_texture;

void main()
{
	frag_color = texture(render_texture, o_tex_coord);
}
