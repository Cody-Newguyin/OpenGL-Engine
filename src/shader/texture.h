#ifndef _SHADER_TEXTURE_H_
#define _SHADER_TEXTURE_H_

#include <glad.h>
#include <glfw3.h>
#include <string>

class Texture {
    public:
    unsigned int ID;

    // Texture Information
    // GLenum target = GL_TEXTURE_2D;
    GLenum internalFormat = GL_RGBA;
    GLenum format = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;
    GLenum filterMin = GL_LINEAR_MIPMAP_LINEAR;
    GLenum filterMax = GL_LINEAR;
    GLenum wrapS = GL_REPEAT;
    GLenum wrapT = GL_REPEAT;  
    bool mipmapEnabled = true;

    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int nrChannels = 0;

    public:
    Texture();

    void DefaultTexture(unsigned int width, unsigned int height, GLenum format, GLenum internalFormat);
    void LoadTexture(std::string filename, GLenum internalFormat);
    void Bind(int unit);
    void Unbind();
};

#endif