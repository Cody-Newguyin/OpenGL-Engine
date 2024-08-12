#ifndef _SHADER_BASIC_MATERIAL_H_
#define _SHADER_BASIC_MATERIAL_H_

#include <string>
#include "shader/material.h"

enum NORM_MAP_TYPE {
    NORM_TYPE_NONE,
    NORM_MAP_NORM,
    NORM_MAP_BUMP,
};

class BasicMaterial : public Material {
    public:
    Shader BasicShader;

    Texture mainTex;
    Texture detailTex;
    Texture bumpMap;
    Texture normalMap;
    NORM_MAP_TYPE type = NORM_TYPE_NONE;

    glm::vec4 mainTex_ST = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 detailTex_ST = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float smoothness = 0.5f;
    float metallic = 0.0f;

    std::string mainFile = "textures/white.png";
    std::string detailFile = "textures/white.png";
    std::string normalFile = "";
    
    public:
    BasicMaterial(std::string name = "text");
    ~BasicMaterial(); 
    void Initalize();

    void SetMainFile(std::string filename);
    void SetDetailFile(std::string filename);
    void SetNormalFile(std::string filename, NORM_MAP_TYPE type);
};

#endif