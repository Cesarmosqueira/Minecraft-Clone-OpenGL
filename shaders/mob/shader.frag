#version 330 core

in vec3 pos;
in vec3 normal;
in vec2 texCoords;

out vec4 color;

uniform vec3 xyz;      // light position
uniform vec3 xyzColor; // light color
uniform vec3 xyzView;  // camera position

uniform vec3 worldPos; // position in the world
uniform float u_starting_ambient;
uniform float u_starting_specular;
uniform float u_minimum_ambient;

uniform sampler2D texture_diffuse1;


float attenuation_formula(float dist){
    //return 1.0/(1.0 + 0.14*dist + 0.7*powf(dist,2));
    return 1.0 / (u_minimum_ambient + 0.1*dist + 0.01*dist*dist);
}

void main() {
    float attenuation = attenuation_formula(distance(xyz, worldPos));


	// ambient
	float strength = u_minimum_ambient;
	vec3 ambient   = strength * xyzColor;
    ambient *= attenuation;

	// diffuse
	vec3  norm     = normalize(normal);
	vec3  lightDir = normalize(xyz - pos);
	float diff     = max(dot(norm, lightDir), 0.0);
	vec3  diffuse  = diff * xyzColor;
    diffuse *= attenuation;

	// specular
	float specularStrength = u_starting_specular;
	vec3  viewDir          = normalize(xyzView - pos);
	vec3  reflectDir       = reflect(-lightDir, norm);
	float spec             = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3  specular         = specularStrength * spec * xyzColor;
    specular *= attenuation;

	vec3 result    = (ambient + diffuse + specular) * vec3(1.0f);
	color          = texture(texture_diffuse1, texCoords) * vec4(result, 1.0f);
}

