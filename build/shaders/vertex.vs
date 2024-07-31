#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 norm;

out Interpolator {
    vec2 uv;
    vec3 normal;
    vec3 worldPos;
    vec4 shadowCoords;
} i;

// globals
uniform mat4 _transform;
uniform mat4 _view;
uniform mat4 _projection;
uniform mat4 _lightSpaceMatrix;

uniform vec4 _mainTex_ST;

void main() {
    i.uv = texCoord * _mainTex_ST.xy + _mainTex_ST.zw;
    i.normal = mat3(transpose(inverse(_transform))) * norm;
    i.worldPos = vec3(_transform * vec4(pos, 1.0));
    i.shadowCoords = _lightSpaceMatrix * vec4(i.worldPos, 1.0);

    gl_Position = _projection * _view * _transform * vec4(pos, 1.0);
}
