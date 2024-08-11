#version 330 core

#include uniforms.glsl

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 norm;
layout (location = 3) in vec4 tangent;

out Input {
    vec2 uv;
    vec3 normal;
    vec3 worldPos;
    vec4 tangent;
} i;

uniform mat4 _transform;
uniform vec4 _mainTex_ST;

void main() {
    i.uv = texCoord * _mainTex_ST.xy + _mainTex_ST.zw;
    i.normal = mat3(transpose(inverse(_transform))) * norm;
    i.worldPos = vec3( vec4(pos, 1.0));
    // i.tangent = vec4((_transform * vec4(tangent.xyz, 0.0)).xyz, tangent.w);
    i.tangent = vec4(normalize(mat3(_transform) * vec3(1.0, 0.0, 0.0)), 1.0);
    
    gl_Position = _projection * _view * _transform * vec4(pos, 1.0);
}
