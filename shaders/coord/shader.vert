#version 330 core
layout (location = 0) in vec3 vertPos;
layout (location = 1) in float vertColor;
layout (location = 2) in vec2 myTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out float fragColor;
out vec2 texCoord;

void main() {
	gl_Position = proj * view * model * vec4(vertPos, 1.0); 
	fragColor = vertColor;
	texCoord = vec2(myTexCoord.x, myTexCoord.y);
}
