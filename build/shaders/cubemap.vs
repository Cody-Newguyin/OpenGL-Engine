#version 330 core
layout (location = 0) in vec3 pos;

out vec3 worldPos;

uniform mat4 _projection;
uniform mat4 _view;

void main()
{
    worldPos = pos;  
    gl_Position =  _projection * _view * vec4(worldPos, 1.0);
}