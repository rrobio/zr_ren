#version 330 core

in vec3 pos;
in vec3 normal;

uniform mat4 model;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

out vec4 finalColor;

void main()
{
    vec3 fragPos = pos;

	// direction from light to surface
	vec3 lightDir = normalize(lightPos - fragPos);

	// compute ambient contribution
	float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

	// compute diffuse contribution
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 shading = (ambient + diffuse) * objectColor; 
    finalColor = vec4(shading, 1.0);
} 