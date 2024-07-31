#include "engine/engine.h"
#include "log/log.h"
#include "mesh/cube.h"
#include "mesh/quad.h"
#include "engine.h"

Engine::Engine() {
    camera = Camera();
}

void Engine::Initialize(GLFWwindow* window, Scene* scene) {
    this->window = window;
    this->scene = scene;

    camera.Initialize();

    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    scene->background = new Background();
    PBRcapture();
    
    // Shadow shit
    shadowShader = new Shader();
    shadowShader->Initialize("shaders/shadow_cast.vs", "shaders/shadow_cast.fs");
    for (unsigned int i = 0; i < 4; i++) {
        glGenFramebuffers(1, &depthMapsFBO[i]);

        Texture* shadowMap = new Texture();
        shadowMap->DefaultTexture(2048, 2048, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
        shadowMaps.push_back(shadowMap);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapsFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap->ID, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Engine::Render() {
    // Geometry buffer
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

    // Render Background
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    // update camera
    camera.ProcessInput(window);
    camera.Update();

    LoadObject2Buffers(scene->root);
    ShadowCapture();
    ResetViewport();
    RenderScene();
    ClearBuffers();
}

void Engine::UpdateGlobalUniforms(Shader* shader) {
    LightObject *light;
    for (int i = 0; i < scene->pointLights.size() && i < 4; i++) {
        light = scene->pointLights[i];
        shader->SetVector("_pointlight" + std::to_string(i) + "_pos", light->worldPos);
        shader->SetVector("_pointlight" + std::to_string(i) + "_color", light->color * light->intensity);
    }
    for (int i = 0; i < scene->dirLights.size() && i < 4; i++) {
        light = scene->dirLights[i];
        shader->SetVector("_dirlight" + std::to_string(i) + "_dir", light->direction);
        shader->SetVector("_dirlight" + std::to_string(i) + "_color", light->color * light->intensity);
    }
}

void Engine::RenderScene() {
    for (unsigned int i = 0; i < meshObjects.size(); i++) {
        RenderObject(meshObjects[i]);
    }
    RenderObject(scene->background);
}

void Engine::LoadObject2Buffers(SceneObject* object) {
    // update _transforms
    object->UpdateTransform();

    // check compenets and add em
    if (object->mesh) {
        meshObjects.push_back(object);
    }

    // Recursively check children
    for (unsigned int i = 0; i < object->children.size(); i++) {
        LoadObject2Buffers(object->children[i]);
    }
}

void Engine::ClearBuffers() {
    meshObjects.clear();
}

void Engine::RenderObject(SceneObject* object) {
    Material *material = object->material;
    Mesh     *mesh     = object->mesh;
    Shader   *shader   = object->material->shader;

    // Update global GL blend state
    glDepthFunc(material->DepthCompare);

    // Draw calls after this use this material's shader
    shader->Use(); 
    
    // Bind uniforms specific to material
    int unit = material->BindUniforms();
    // MOVE THIS AS WELL 
    // I want to make this global but unit is dependent on material
    shader->SetInt("_irrMap", irrMap->ID);
    irrMap->Bind(unit);
    shader->SetInt("_prefilterMap", prefilterMap->ID);
    prefilterMap->Bind(++unit);
    shader->SetInt("_brdfLUT", brdfLUTTexture->ID);
    brdfLUTTexture->Bind(++unit);
    // not dynamic yet
    shader->SetInt("_shadowMap0", shadowMaps[0]->ID);
    shadowMaps[0]->Bind(++unit);
    shader->SetMatrix("_lightSpaceMatrix", scene->dirLights[0]->spaceMatrix);

    // Update shader matrices to apply _transformations
    shader->SetMatrix("_transform", object->transform);
    shader->SetMatrix("_projection", camera.projection);
    shader->SetMatrix("_view", camera.view);
    shader->SetVector("_camPos", camera.position);
    
    // Update global uniforms 
    UpdateGlobalUniforms(shader);
    
    // Finally draw the mesh
    RenderMesh(mesh);
}

void Engine::RenderMesh(Mesh* mesh) {
    // Bind VAO
    glBindVertexArray(mesh->VAO);
    // Draw verticies
    if (mesh->EBOenabled) {
        glDrawElements(mesh->primitive, static_cast<unsigned int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(mesh->primitive, 0, static_cast<unsigned int>(mesh->vertices.size()));
    }
    // Unbind VAO
    glBindVertexArray(0);
}

void Engine::ShadowCapture() {
    LightObject *light;
    glm::mat4 lightProjection, lightView, lightSpaceMatrix;
    lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -15.0f, 20.0f);

    for (int i = 0; i < scene->dirLights.size() && i < 4; i++) {
        light = scene->dirLights[i];

        //LOG_GLM(light->direction * 5.0f);
        lightView = glm::lookAt(light->direction * 5.0f, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        light->spaceMatrix = lightSpaceMatrix;

        glViewport(0, 0, shadowMaps[i]->width, shadowMaps[i]->height);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapsFBO[i]);
        glClear(GL_DEPTH_BUFFER_BIT);

        shadowShader->Use();
        shadowShader->SetMatrix("_lightSpaceMatrix", lightSpaceMatrix);
        for (unsigned int i = 0; i < meshObjects.size(); i++) {
            shadowShader->SetMatrix("_transform", meshObjects[i]->transform);
            RenderMesh(meshObjects[i]->mesh);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::PBRcapture() {
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  

    irrMap = new TextureCube();
    irrMap->DefaultTextureCube(32, 32, GL_RGB, GL_RGB);

    Shader* irrShader = new Shader();
    irrShader->Initialize("shaders/cubemap.vs", "shaders/irradiance_capture.fs");

    Material* irrCapture = new Material();
    irrCapture->SetTextureCube("_envMap", scene->background->envMap);
    irrCapture->SetShader(irrShader);
    irrCapture->DepthCompare = GL_LEQUAL;

    SceneObject envCube = SceneObject(new Cube(), irrCapture);
    Render2CubeMap(&envCube, irrMap, 0);

    prefilterMap = new TextureCube();
    prefilterMap->filterMin = GL_LINEAR_MIPMAP_LINEAR;
    prefilterMap->mipmapEnabled = true;
    prefilterMap->DefaultTextureCube(128, 128, GL_RGB, GL_RGB);

    Shader* prefilterShader = new Shader();
    prefilterShader->Initialize("shaders/cubemap.vs", "shaders/prefilter_capture.fs");

    Material* prefilterCapture = new Material();
    prefilterCapture->SetTextureCube("_envMap", scene->background->envMap);
    prefilterCapture->SetShader(prefilterShader);
    prefilterCapture->DepthCompare = GL_LEQUAL;

    envCube.material = prefilterCapture;
    unsigned int maxMipLevels = 5;
    for (unsigned int i = 0; i < maxMipLevels; i++) {
        float roughness = (float)i / (float)(maxMipLevels - 1);
        prefilterCapture->SetFloat("roughness", &roughness);
        Render2CubeMap(&envCube, prefilterMap, i);
    }

    brdfLUTTexture = new Texture();
    brdfLUTTexture->DefaultTexture(128, 128, GL_RG, GL_RG);

    Shader* brdfShader = new Shader();
    brdfShader->Initialize("shaders/screenquad.vs", "shaders/integrateBRDF.fs");

    Material* brdfCapture = new Material();
    brdfCapture->SetShader(brdfShader);

    SceneObject brdfQuad = SceneObject(new Quad(), brdfCapture);
    Render2Texture(&brdfQuad, brdfLUTTexture);
}

void Engine::Render2Texture(SceneObject* object, Texture* target) {
    Material *material = object->material;
    Mesh     *mesh     = object->mesh;
    Shader   *shader   = object->material->shader;
    
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, target->width, target->height);

    glViewport(0, 0, target->width, target->height);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    shader->Use();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target->ID, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderMesh(mesh);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Engine::Render2CubeMap(SceneObject* envCube, TextureCube* target, unsigned int mipLevel) {
    // Render a cube with a texture then capture the results 6 times for each face
    // Results are stored in a framebuffer which is then stored in target cubemap
    Material *material = envCube->material;
    Mesh     *mesh     = envCube->mesh;
    Shader   *shader   = envCube->material->shader;

    // resize target 
    float width = (float)target->width * pow(0.5, mipLevel);
    float height = (float)target->height * pow(0.5, mipLevel);

    // Update global GL blend state
    glDepthFunc(material->DepthCompare);
    // Draw calls after this use this material's shader
    shader->Use(); 
    material->BindUniforms();

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    shader->SetMatrix("_projection", captureProjection);
    for (unsigned int i = 0; i < 6; i++) {
        shader->SetMatrix("_view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, target->ID, mipLevel);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderMesh(mesh);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::ResetViewport() {
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);
}
