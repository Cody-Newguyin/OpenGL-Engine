#version 330 core

out vec4 FragColor;

// globals
uniform vec3 _pointlight0_color;

void main()
{
    FragColor = vec4(_pointlight0_color, 1.0f);
}