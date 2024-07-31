#ifndef _SHADER_TEXTURECUBE_H_
#define _SHADER_TEXTURECUBE_H_

#include <glad.h>
#include <glfw3.h>
#include <string>
#include <vector>

// Cube Map
class TextureCube {
    public:
    unsigned int ID;

    // Texture Information
    // GLenum target = GL_TEXTURE_2D;
    GLenum internalFormat = GL_RGBA;
    GLenum format = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;
    GLenum filterMin = GL_LINEAR;
    GLenum filterMax = GL_LINEAR;
    GLenum wrapS = GL_CLAMP_TO_EDGE;
    GLenum wrapT = GL_CLAMP_TO_EDGE;  
    bool mipmapEnabled = false;;

    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int nrChannels = 0;

    public:
    TextureCube();

    void DefaultTextureCube(unsigned int width, unsigned int height, GLenum format, GLenum internalFormat);
    void LoadTextureCube(std::vector<std::string> filenames, GLenum internalFormat);
    void Bind(int unit);
    void Unbind();
};

#endif