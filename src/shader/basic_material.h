#ifndef _SHADER_BASIC_MATERIAL_H_
#define _SHADER_BASIC_MATERIAL_H_

#include <string>
#include "shader/material.h"

enum NORM_MAP_TYPE {
    NORM_MAP_NONE,
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
    Texture metallicMap;
    Texture smoothnessMap;
    Texture ambientOcclusionMap;
    Texture emissionMap;

    // Texture loading configurations
    NORM_MAP_TYPE type = NORM_MAP_NONE;
    bool flipImage = false;

    glm::vec4 mainTex_ST = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 detailTex_ST = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float smoothness = 0.5f;
    float metallic = 0.0f;
    float bumpScale = 1.0f;
    float ambient = 0.2f;

    std::string mainFile = "textures/white.png";
    std::string detailFile = "textures/white.png";
    std::string normalFile = "";
    std::string metallicFile = "";
    std::string smoothnessFile = "";
    std::string ambientOcclusionFile = "";
    std::string emissionFile = "";
    
    public:
    BasicMaterial(std::string name = "text");
    ~BasicMaterial(); 
    void Initalize();

    void SetFlipImage(bool flipImage);
    void SetMainFile(std::string filename);
    void SetDetailFile(std::string filename);
    void SetNormalFile(std::string filename, NORM_MAP_TYPE type);
    void SetMetallicFile(std::string filename);
    void SetSmoothnessFile(std::string filename);
    void SetAmbientOcclustionFile(std::string filename);
    void SetEmissionFile(std::string filename);
};

#endif