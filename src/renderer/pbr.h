#ifndef _PBR_H_
#define _PBR_H_

#include <glad.h>
#include <glfw3.h>
#include <vector>

#include "renderer/renderer.h"
#include "shader/texture.h"
#include "shader/texture_cube.h"
#include "shader/shader.h"
#include "shader/material.h"
#include "scene/scene_object.h"
#include "mesh/cube.h"
#include "mesh/quad.h"

class Renderer;

class PBR {
    public:
    Renderer* renderer;
    TextureCube* irrMap;
    TextureCube* prefilterMap;
    Texture* brdfLUTTexture;

    public:
    PBR(Renderer* renderer);

    private:
};

#endif