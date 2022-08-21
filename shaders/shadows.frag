#version 330 core
out vec4 frag_color;

struct Light {
	vec3 position;
};

in VS_OUT {
   vec3 pos;
   vec3 normal;
   vec2 tex_coords;
} fs_in;

uniform	sampler2D diffuse_texture;
uniform	samplerCube depth_map;
uniform Light light;
uniform vec3 view_pos;

uniform float far_plane;

float shadow_calculation(vec3 frag_pos)
{
    // get vector between fragment position and light position
    vec3 frag_to_light = frag_pos - light.position;
    // ise the fragment to light vector to sample from the depth map
    float closest_depth = texture(depth_map, frag_to_light).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closest_depth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float current_depth = length(frag_to_light);
    // test for shadows
    float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = current_depth -  bias > closest_depth ? 1.0 : 0.0;
    // display closestDepth as debug (to visualize depth cubemap)
	frag_color = vec4(vec3(closest_depth / far_plane), 1.0);

    return shadow;
}

void main()
{
	vec3 color = texture(diffuse_texture, fs_in.tex_coords).rgb;
	vec3 norm = normalize(fs_in.normal);
	vec3 light_color = vec3(0.3);

	vec3 ambient = 0.3 * color;

	vec3 light_dir = normalize(light.position - fs_in.pos);
	float diff = max(dot(light_dir, norm), 0.0);
	vec3 diffuse = diff * light_color;

	vec3 view_dir = normalize(view_pos - fs_in.pos);
	vec3 reflect_dir = reflect(-light_dir, norm);

	vec3 halfway_dir = normalize(light_dir + view_dir);
	float spec = pow(max(dot(norm, halfway_dir), 0.0), 64.0);
	vec3 specular = spec * light_color;


	float bias = max(0.05 * (1.0 - dot(norm, light_dir)), 0.005);
	float shadow = shadow_calculation(fs_in.pos);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

	//	vec3 res = ambient + diffuse + specular;
	frag_color = vec4(lighting, 1.0);
}
