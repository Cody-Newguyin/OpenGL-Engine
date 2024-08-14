#include "scene/background.h"
#include "mesh/cube.h"
#include "shader/shader.h"
#include "shader/material.h"
#include <vector>
#include <string>
#include "background.h"

Background::Background() {
    this->mesh = new Cube();
    this->material = new Material();
    this->envMap = new TextureCube();

    Shader* shader = new Shader();
    shader->Initialize("shaders/background.vs", "shaders/background.fs");

    std::vector<std::string> faces {
        "textures/skybox/right.jpg",
        "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",
        "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg",
        "textures/skybox/back.jpg",
    };
    envMap->LoadTextureCube(faces, GL_RGBA);

    material->SetTextureCube("_envMap", envMap);
    material->SetShader(shader);
    material->DepthCompare = GL_LEQUAL;

    SetName("background");
}

void Background::SetEnvMap(TextureCube* envMap) {
    this->envMap = envMap;
    material->SetTextureCube("_envMap", envMap);
    material->SetShader(this->material->shader);
}