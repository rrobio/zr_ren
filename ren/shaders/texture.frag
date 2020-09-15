#version 330 core
out vec4 frag_color;

in vec2 tex_coord;
in vec3 pos;
in vec3 normal;

uniform sampler2D u_texture;

uniform vec3 view_pos;
uniform vec3 light_pos;
uniform vec3 light_color;

void main()
{
	float ambient_str = 0.1;
	vec3 ambient = ambient_str * light_color;

	vec3 norm = normalize(normal);
	vec3 light_dir = normalize(light_pos - pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = diff * light_color;

	float specular_str = 0.5;
	vec3 view_dir = normalize(view_pos - pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = specular_str * spec * light_color;

	vec3 res = (ambient + diffuse + specular);
	frag_color = vec4(res, 1.0) * texture(u_texture, tex_coord);
}
