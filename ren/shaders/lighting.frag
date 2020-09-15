#version 330 core
out vec4 frag_color;

struct Material {
	sampler2D diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in VS_OUT {
   vec3 pos;
   vec3 normal;
   vec2 tex_coords;
} fs_in;

uniform vec3 view_pos;
uniform Material material;
uniform Light light;

void main()
{
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.tex_coords));

	vec3 norm = normalize(fs_in.normal);
	vec3 light_dir = normalize(light.position - fs_in.pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.tex_coords));

	vec3 view_dir = normalize(view_pos - fs_in.pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	vec3 res = ambient + diffuse + specular;
	frag_color = vec4(res, 1.0);
}
