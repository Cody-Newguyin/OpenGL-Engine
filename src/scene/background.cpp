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
        "textures/roombox/right.png",
        "textures/roombox/left.png",
        "textures/roombox/top.png",
        "textures/roombox/bottom.png",
        "textures/roombox/front.png",
        "textures/roombox/back.png",
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