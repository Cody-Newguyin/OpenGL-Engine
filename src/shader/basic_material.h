#ifndef _SHADER_BASIC_MATERIAL_H_
#define _SHADER_BASIC_MATERIAL_H_

#include <string>
#include "shader/material.h"

class BasicMaterial : public Material {
    public:
    Shader BasicShader;

    Texture mainTex;
    Texture detailTex;

    glm::vec4 mainTex_ST = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
    float smoothness = 0.5f;
    float metallic = 0.0f;

    public:
    BasicMaterial(std::string mainFile = "textures/white.png");
    ~BasicMaterial();
};

#endif