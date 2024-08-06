#version 330 core
layout (location = 0) in vec3 pos;

uniform mat4 _shadowTransform;
uniform mat4 _transform;

out vec3 worldPos;

void main() {
    gl_Position = _shadowTransform * _transform * vec4(pos, 1.0);
    worldPos = vec3(_transform * vec4(pos, 1.0));
}  