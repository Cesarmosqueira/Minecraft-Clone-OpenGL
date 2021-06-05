#version 330 core
layout (location = 0) in vec3  vertPos;
layout (location = 1) in float shadowModifier;
layout (location = 2) in vec3  texCoord;
layout (location = 3) in vec3  vertNormal;

out vec3 fragPos;
out vec3 o_texCoord;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	fragPos     = vec3(model * vec4(vertPos, 1.0));
	o_texCoord  = texCoord;
	Normal      = mat3(transpose(inverse(model))) * vertNormal;

	gl_Position = proj * view * model * vec4(vertPos, 1.0);
}
