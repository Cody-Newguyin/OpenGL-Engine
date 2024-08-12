#include "shader/basic_material.h"
#include "log/log.h"
#include "basic_material.h"

BasicMaterial::BasicMaterial(std::string name) {
   this->name = name;
}

BasicMaterial::~BasicMaterial() {
    BasicShader.Unload();
}

void BasicMaterial::Initalize() {
     // Load Shader
    BasicShader = Shader();

    // Set defines based on type of map being sampled (normal map vs height map)
    if (!normalFile.empty()) {
        if (type == NORM_MAP_NORM) {
            BasicShader.AddDefine("NORMAL_MAP");
        } else if (type == NORM_MAP_BUMP) {
            BasicShader.AddDefine("BUMP_MAP");
        }
    }
    BasicShader.Initialize("shaders/vertex.vs", "shaders/fragment.fs");

    // Load Textures
    mainTex = Texture();
    mainTex.LoadTexture(mainFile, GL_RGBA);
    SetTexture("_mainTex", &mainTex);

    detailTex = Texture();
    detailTex.LoadTexture(detailFile, GL_RED);
    SetTexture("_detailTex", &detailTex);

    if (!normalFile.empty()) {
        if (type == NORM_MAP_NORM) {
            normalMap = Texture();
            normalMap.LoadTexture(normalFile, GL_RGB);
            SetTexture("_normalMap", &normalMap);
        } else if (type == NORM_MAP_BUMP) {
            bumpMap = Texture();
            bumpMap.LoadTexture(normalFile, GL_RED);
            SetTexture("_bumpMap", &bumpMap);
        }
    }

    // Finalize
    SetVector("_color", &color);
    SetVector("_mainTex_ST", &mainTex_ST);
    SetVector("_detailTex_ST", &detailTex_ST);
    SetFloat("_smoothness", &smoothness);
    SetFloat("_metallic", &metallic);
    SetShader(&BasicShader);
}


void BasicMaterial::SetMainFile(std::string filename) {
    this->mainFile = filename;
}

void BasicMaterial::SetDetailFile(std::string filename) {
    this->detailFile = filename;
}

void BasicMaterial::SetNormalFile(std::string filename, NORM_MAP_TYPE type) {
    this->normalFile = filename;
    this->type = type;
}
