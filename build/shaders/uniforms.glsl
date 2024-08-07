#ifndef _UNIFORMS_H
#define _UNIFORMS_H

layout (std140) uniform Globals {
    mat4 _projection;
    mat4 _view;
    vec3 _camPos;
};

#endif