#version 330 core
layout (location = 0) in vec3 pos;

uniform mat4 _transform;

void main() {
    gl_Position = _transform * vec4(pos, 1.0);
}  