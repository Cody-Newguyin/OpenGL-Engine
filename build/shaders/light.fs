#version 330 core

#include uniforms.glsl

out vec4 FragColor;

void main() {
    FragColor = vec4(_pointlight_color[0], 1.0f);
}