#version 330 core
in vec2 texCoord;

uniform sampler2D texture1;

out vec4 color;
void main() {
    color = texture(texture1, texCoord);
}
