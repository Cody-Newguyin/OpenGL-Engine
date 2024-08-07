#version 330 core

#include uniforms.glsl

layout (location = 0) in vec3 pos;

uniform mat4 _transform;

void main() {
    gl_Position = _projection * _view * _transform * vec4(pos, 1.0);
}
