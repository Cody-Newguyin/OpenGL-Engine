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

    // Load texures and set defines based on type of map being sampled
    mainTex = Texture();
    mainTex.LoadTexture(mainFile, GL_SRGB, flipImage);
    SetTexture("_mainTex", &mainTex);

    detailTex = Texture();
    detailTex.LoadTexture(detailFile, GL_RED, flipImage);
    SetTexture("_detailTex", &detailTex);

    if (!normalFile.empty()) {
        if (normType == NORM_MAP_NORM) {
            BasicShader.AddDefine("NORMAL_MAP");
            normalMap = Texture();
            normalMap.LoadTexture(normalFile, GL_RGB, flipImage);
            SetTexture("_normalMap", &normalMap);
        } else if (normType == NORM_MAP_BUMP) {
            BasicShader.AddDefine("BUMP_MAP");
            normalMap = Texture();
            normalMap.LoadTexture(normalFile, GL_RED, flipImage);
            SetTexture("_bumpMap", &normalMap);
        }
        SetFloat("_bumpScale", &bumpScale);
    }
    if (!metallicFile.empty()) {
        BasicShader.AddDefine("METALLIC_MAP");
        metallicMap = Texture();
        metallicMap.LoadTexture(metallicFile, GL_RED, flipImage);
        SetTexture("_metallicMap", &metallicMap);
    } 
    if (!smoothnessFile.empty()) {
        if (smoothType == SMOOTH_MAP_SMOOTH) {
            BasicShader.AddDefine("SMOOTHNESS_MAP");
            smoothnessMap = Texture();
            smoothnessMap.LoadTexture(smoothnessFile, GL_RED, flipImage);
            SetTexture("_smoothnessMap", &smoothnessMap);
        } else if (smoothType == SMOOTH_MAP_ROUGH) {
            BasicShader.AddDefine("ROUGHNESS_MAP");
            smoothnessMap = Texture();
            smoothnessMap.LoadTexture(smoothnessFile, GL_RED, flipImage);
            SetTexture("_roughnessMap", &smoothnessMap);
        }
        
    } 
    if (!ambientOcclusionFile.empty()) {
        BasicShader.AddDefine("OCCLUSION_MAP");
        ambientOcclusionMap = Texture();
        ambientOcclusionMap.LoadTexture(ambientOcclusionFile, GL_RED, flipImage);
        SetTexture("_occlusionMap", &ambientOcclusionMap);
    }
    if (!emissionFile.empty()) {
        BasicShader.AddDefine("EMISSION_MAP");
        emissionMap = Texture();
        emissionMap.LoadTexture(emissionFile, GL_RGB, flipImage);
        SetTexture("_emissionMap", &emissionMap);
    }

    // Finalize
    SetVector("_color", &color);
    SetVector("_mainTex_ST", &mainTex_ST);
    SetVector("_detailTex_ST", &detailTex_ST);
    SetFloat("_smoothness", &smoothness);
    SetFloat("_metallic", &metallic);
    SetFloat("_ambient", &ambient);
    
    BasicShader.Initialize("shaders/vertex.vs", "shaders/fragment.fs");
    SetShader(&BasicShader);
}

void BasicMaterial::SetFlipImage(bool flipImage) {
    this->flipImage = flipImage;
}

void BasicMaterial::SetMainFile(std::string filename)
{
    this->mainFile = filename;
}

void BasicMaterial::SetDetailFile(std::string filename) {
    this->detailFile = filename;
}

void BasicMaterial::SetNormalFile(std::string filename, NORM_MAP_TYPE type) {
    this->normalFile = filename;
    this->normType = type;
}

void BasicMaterial::SetMetallicFile(std::string filename) {
    this->metallicFile = filename;
}

void BasicMaterial::SetSmoothnessFile(std::string filename, SMOOTH_MAP_TYPE type) {
    this->smoothnessFile = filename;
    this->smoothType = type;
}

void BasicMaterial::SetAmbientOcclustionFile(std::string filename) {
    this->ambientOcclusionFile = filename;
}

void BasicMaterial::SetEmissionFile(std::string filename) {
    this->emissionFile = filename;
}
