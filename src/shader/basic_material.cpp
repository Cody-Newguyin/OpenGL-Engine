#include "shader/basic_material.h"

BasicMaterial::BasicMaterial(std::string mainFile) {
    // Load Shader
    BasicShader = Shader();
    BasicShader.Initialize("shaders/vertex.vs", "shaders/fragment.fs");

    // Load Textures
    mainTex = Texture();
    mainTex.LoadTexture(mainFile, GL_RGBA);
    detailTex = Texture();
    detailTex.LoadTexture("textures/grid.png", GL_RED);
    
    // Finalize
    SetTexture("_mainTex", &mainTex);
    SetTexture("_detailTex", &detailTex);
    SetVector("_mainTex_ST", &mainTex_ST);
    SetFloat("_smoothness", &smoothness);
    SetFloat("_metallic", &metallic);
    SetShader(&BasicShader);
}

BasicMaterial::~BasicMaterial() {
    BasicShader.Unload();
}