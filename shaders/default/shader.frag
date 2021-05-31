#version 330 core
in float fragColor;
in vec2 texCoord;

uniform sampler2D texture1;

out vec4 color;
void main() {
    color = texture(texture1, texCoord);
    color.rgb *= vec3(fragColor);
}
