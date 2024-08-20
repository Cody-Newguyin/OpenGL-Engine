#include "render_target.h"

RenderTarget::RenderTarget(unsigned int width, unsigned int height, GLenum format, GLenum internalFormat) {
    this->width = width;
    this->height = height;

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    texture = Texture();
    texture.DefaultTexture(width, height, format, internalFormat);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.ID, 0);

    glGenRenderbuffers(1, & RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}