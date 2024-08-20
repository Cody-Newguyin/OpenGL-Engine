#ifndef _RENDER_TARGET_H_
#define _RENDER_TARGET_H_

#include <glad.h>
#include <glfw3.h>
#include <vector>
#include "shader/texture.h"
#include "shader/shader.h"

class RenderTarget {
    public:
    unsigned int FBO;
    unsigned int RBO;
    unsigned int width, height;
    Texture texture;

    public:
    RenderTarget(unsigned int width, unsigned int height, GLenum format, GLenum internalFormat);
};

#endif