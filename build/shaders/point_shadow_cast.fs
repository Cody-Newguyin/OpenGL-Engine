#version 330 core
in vec4 FragPos;

uniform vec3 _pointlight0_pos;
uniform float _farPlane;

void main() {
    float lightDistance = length(FragPos.xyz - _pointlight0_pos);
    
    // map to value between 0 and 1
    lightDistance = lightDistance / _farPlane;

    // write to depth
    gl_FragDepth = 0.5f;
}  