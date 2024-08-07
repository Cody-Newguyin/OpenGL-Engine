#version 330 core

#include uniforms.glsl

layout (location = 0) in vec3 pos;

out vec3 worldPos;

void main() {
    worldPos = pos;
    // remove translating from view matrix
    mat4 rotView = mat4(mat3(_view));
    vec4 clipPos = _projection * rotView * vec4(pos, 1.0);

    gl_Position = clipPos.xyww;
}
