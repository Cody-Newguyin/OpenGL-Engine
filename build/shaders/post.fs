#version 330 core

out vec4 FragColor;
in vec2 uv;

uniform sampler2D _screen;

void main() {
    FragColor = vec4(1.0) - texture(_screen, uv);
}