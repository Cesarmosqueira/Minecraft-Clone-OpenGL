#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 pos;
out vec3 normal;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	pos       = vec3(model * vec4(aPos, 1.0));
	normal    = mat3(transpose(inverse(model))) * aNormal;
	texCoords = aTexCoords;

	gl_Position = proj * view * vec4(pos, 1.0);
}


