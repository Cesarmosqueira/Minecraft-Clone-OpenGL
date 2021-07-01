#version 330 core

in vec3 fragPos;
in vec2 o_texCoord;
in vec3 Normal;

uniform vec3 xyz;      // light position
uniform vec3 xyzColor; // light color
uniform vec3 xyzView;  // camera position
uniform vec3 xyzBlock;

uniform float u_starting_ambient;
uniform float u_starting_specular;
uniform float u_minimum_ambient;

out vec4 color;
uniform sampler2D texture1;
// PHONG LIGHT

float attenuation_formula(float dist){
    //return 1.0/(1.0 + 0.14*dist + 0.7*powf(dist,2));
    return 1.0 / (u_minimum_ambient + 0.1*dist + 0.01*dist*dist);
}

void main() {
    float attenuation = attenuation_formula(distance(xyz, xyzBlock));

	// ambient
	float strength = u_starting_ambient;
	vec3 ambient   = strength * xyzColor;
    ambient *= attenuation;

	// diffuse
	vec3  norm     = normalize(Normal);
	vec3  lightDir = normalize(xyz - fragPos);
	float diff     = max(dot(norm, lightDir), 0.0);
	vec3  diffuse  = diff * xyzColor;
    diffuse *= attenuation;

	// specular
	float specularStrength = u_starting_specular;
	vec3  viewDir          = normalize(xyzView - fragPos);
	vec3  reflectDir       = reflect(-lightDir, norm);
	float spec             = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3  specular         = specularStrength * spec * xyzColor;
    specular *= attenuation;

    // total (phong)
	vec3 result    = (ambient + diffuse + specular) * vec3(texture(texture1, o_texCoord));
	color          = vec4(result, 1.0);
}
