#version 330 core
uniform vec4 color;
out vec4 fragColor;

void main() {
    fragColor = color;
    fragColor = vec4(1, 0, 0, 1);
}