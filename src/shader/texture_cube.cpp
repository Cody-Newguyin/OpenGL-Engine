#include "shader/texture_cube.h"
#include "log/log.h"
#include <stb_image.h>
#include "texture_cube.h"

TextureCube::TextureCube() {

}

void TextureCube::DefaultTextureCube(unsigned int width, unsigned int height, GLenum format, GLenum internalFormat) {
    // generate id for texture
    glGenTextures(1, &ID);
    // all upcoming GL_TEXTURE__CUBE_MAP operations now have effect on this texture object
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID); 

    this->internalFormat = internalFormat;

    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, GL_FLOAT, NULL);
    }
    if (mipmapEnabled) {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
    
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filterMax);

    // unbind texture
    Unbind();

    // set dimensions only when texture was successfully set
    this->width = width;
    this->height = height;
}

void TextureCube::LoadTextureCube(std::vector<std::string> filenames, GLenum internalFormat) {
    // generate id for texture
    
    glGenTextures(1, &ID);
    // all upcoming GL_TEXTURE__CUBE_MAP operations now have effect on this texture object
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID); 

    this->internalFormat = internalFormat;

    // DONT flip images before loading
    stbi_set_flip_vertically_on_load(false); 

    // load images and set texture configurations
    int width, height, nrChannels;
    for (unsigned int i = 0; i < filenames.size() && i < 6; i++) {
        unsigned char *data = stbi_load(filenames[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        } else {
            LOG_ERROR("Failed to load texture on file(" + filenames[i] + ")");
        }
        stbi_image_free(data);
    }

    if (mipmapEnabled) {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filterMax);

    // unbind texture
    Unbind();

    // set dimensions only when texture was successfully set
    this->width = width;
    this->height = height;
}

void TextureCube::Bind(int unit) {
    if (unit >= 0) {
        glActiveTexture(GL_TEXTURE0 + unit);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void TextureCube::Unbind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
