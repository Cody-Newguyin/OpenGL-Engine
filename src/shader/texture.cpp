#include "shader/texture.h"
#include "log/log.h"
#include <stb_image.h>
#include "texture.h"

Texture::Texture() {

}

void Texture::DefaultTexture(unsigned int width, unsigned int height, GLenum format, GLenum internalFormat) {
    // generate id for texture
    glGenTextures(1, &ID);
    // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    glBindTexture(GL_TEXTURE_2D, ID); 

    this->mipmapEnabled = false;
    this->filterMin = GL_LINEAR;
    this->wrapS = GL_CLAMP_TO_EDGE;
    this->wrapT = GL_CLAMP_TO_EDGE;
    this->internalFormat = internalFormat;

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, 0);
    if (mipmapEnabled) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMax);

    // unbind texture
    Unbind();

    // set dimensions only when texture was successfully set
    this->width = width;
    this->height = height;
}

void Texture::LoadTexture(std::string filename, GLenum internalFormat) {
    // generate id for texture
    glGenTextures(1, &ID);
    // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    glBindTexture(GL_TEXTURE_2D, ID); 
    
    this->internalFormat = internalFormat;

    // Flip images before loading
    stbi_set_flip_vertically_on_load(false); 

    // load image and set texture configurations
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    if (!data) {
        LOG_ERROR("Failed to load texture on file(" + filename + ")");
        stbi_image_free(data);
        return;
    }

    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    if (mipmapEnabled) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMax);

    // unbind texture
    Unbind();

    // set dimensions only when texture was successfully set
    this->width = width;
    this->height = height;

    stbi_image_free(data);
}

void Texture::Bind(int unit) {
    if (unit >= 0) {
        glActiveTexture(GL_TEXTURE0 + unit);
    }
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
