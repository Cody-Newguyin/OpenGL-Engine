#ifndef _UNIFORMS_H
#define _UNIFORMS_H

const int n_lights = 4;
const int n_cascades = 4;

layout (std140) uniform Globals {
    mat4 _projection;
    mat4 _view;
    vec3 _camPos;
    float _planes[n_cascades + 1];
    vec3 _dirlight_dir[n_lights];
    vec3 _dirlight_color[n_lights];
    mat4 _lightSpaceMatrices[n_lights * n_cascades];
    vec3 _pointlight_pos[n_lights];
    vec3 _pointlight_color[n_lights];
};

#endif