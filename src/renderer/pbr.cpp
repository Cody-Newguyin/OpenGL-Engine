#include "pbr.h"

PBR::PBR(Renderer* renderer) {
    this->renderer = renderer;

    irrMap = new TextureCube();
    irrMap->DefaultTextureCube(64, 64, GL_RGB, GL_RGB);

    Shader* irrShader = new Shader();
    irrShader->Initialize("shaders/cubemap.vs", "shaders/irradiance_capture.fs");

    Material* irrCapture = new Material();
    irrCapture->SetTextureCube("_envMap", renderer->scene->background->envMap);
    irrCapture->SetShader(irrShader);
    irrCapture->DepthCompare = GL_LEQUAL;

    SceneObject envCube = SceneObject(new Cube(), irrCapture);
    renderer->Render2CubeMap(&envCube, irrMap, 0);

    prefilterMap = new TextureCube();
    prefilterMap->filterMin = GL_LINEAR_MIPMAP_LINEAR;
    prefilterMap->mipmapEnabled = true;
    prefilterMap->DefaultTextureCube(256, 256, GL_RGB, GL_RGB);

    Shader* prefilterShader = new Shader();
    prefilterShader->Initialize("shaders/cubemap.vs", "shaders/prefilter_capture.fs");

    Material* prefilterCapture = new Material();
    prefilterCapture->SetTextureCube("_envMap", renderer->scene->background->envMap);
    prefilterCapture->SetShader(prefilterShader);
    prefilterCapture->DepthCompare = GL_LEQUAL;

    envCube.material = prefilterCapture;
    unsigned int maxMipLevels = 5;
    for (unsigned int i = 0; i < maxMipLevels; i++) {
        float roughness = (float)i / (float)(maxMipLevels - 1);
        prefilterCapture->SetFloat("roughness", &roughness);
        renderer->Render2CubeMap(&envCube, prefilterMap, i);
    }

    brdfLUTTexture = new Texture();
    brdfLUTTexture->DefaultTexture(128, 128, GL_RG, GL_RG);

    Shader* brdfShader = new Shader();
    brdfShader->Initialize("shaders/screenquad.vs", "shaders/integrateBRDF.fs");

    Material* brdfCapture = new Material();
    brdfCapture->SetShader(brdfShader);

    SceneObject brdfQuad = SceneObject(new Quad(), brdfCapture);
    renderer->Render2Texture(&brdfQuad, brdfLUTTexture);
}
