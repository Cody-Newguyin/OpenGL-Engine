#include "shader/basic_material.h"
#include "log/log.h"

BasicMaterial::BasicMaterial(std::string mainFile, std::string normalFile, NORM_MAP_TYPE type) {
    // Load Shader
    BasicShader = Shader();
    if (!normalFile.empty()) {
        if (type = NORM_MAP_NORM) {
            BasicShader.AddDefine("NORMAL_MAP");
        } else if (type = NORM_MAP_BUMP) {
            BasicShader.AddDefine("BUMP_MAP");
        }
    }
    BasicShader.Initialize("shaders/vertex.vs", "shaders/fragment.fs");

    // Load Textures
    mainTex = Texture();
    mainTex.LoadTexture(mainFile, GL_RGBA);
    SetTexture("_mainTex", &mainTex);

    detailTex = Texture();
    detailTex.LoadTexture("textures/grid.png", GL_RED);
    SetTexture("_detailTex", &detailTex);

    if (!normalFile.empty()) {
        if (type = NORM_MAP_NORM) {
            normalMap = Texture();
            normalMap.LoadTexture(normalFile, GL_RGB);
            SetTexture("_normalMap", &normalMap);
            // LOG_INFO("normal map set");
        } else if (type = NORM_MAP_BUMP) {
            bumpMap = Texture();
            bumpMap.LoadTexture(normalFile, GL_RED);
            SetTexture("_bumpMap", &bumpMap);
            // LOG_INFO("bump map set");
        }
    }

    // Finalize
    SetVector("_mainTex_ST", &mainTex_ST);
    SetFloat("_smoothness", &smoothness);
    SetFloat("_metallic", &metallic);
    SetShader(&BasicShader);
}

BasicMaterial::~BasicMaterial() {
    BasicShader.Unload();
}