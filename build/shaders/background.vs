#version 330 core

layout (location = 0) in vec3 pos;

out vec3 worldPos;

// globals
uniform mat4 _view;
uniform mat4 _projection;

void main() {
    worldPos = pos;
    // remove translationg from view matrix
    mat4 rotView = mat4(mat3(_view));
    vec4 clipPos = _projection * rotView * vec4(pos, 1.0);

    gl_Position = clipPos.xyww;
}
