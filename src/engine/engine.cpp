#include "engine/engine.h"
#include "log/log.h"
#include "mesh/cube.h"
#include "mesh/quad.h"
#include "engine.h"
#include <cfloat>

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
    ShadowSetup();

    glGenBuffers(1, &globalUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, globalUBO);
    glBufferData(GL_UNIFORM_BUFFER, 720, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalUBO);
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

    scene->Update();
    LoadObject2Buffers(scene->root);
    ShadowCapture();
    ResetViewport();
    RenderScene();
    ClearBuffers();
}

void Engine::UpdateGlobalUniforms(Shader* shader) {
    glBindBuffer(GL_UNIFORM_BUFFER, globalUBO);
    glBufferSubData(GL_UNIFORM_BUFFER,   0, sizeof(glm::mat4), &camera.projection); 
    glBufferSubData(GL_UNIFORM_BUFFER,  64, sizeof(glm::mat4), &camera.view); 
    glBufferSubData(GL_UNIFORM_BUFFER, 128, sizeof(glm::vec3), &camera.position);

    // update light information
    LightObject *light;
    int base = 144;
    for (unsigned int i = 0; i < scene->dirLights.size() && i < n_lights; i++) {
        light = scene->dirLights[i];
        glBufferSubData(GL_UNIFORM_BUFFER, base + i * 16,                       sizeof(glm::vec3), &light->direction);
        glBufferSubData(GL_UNIFORM_BUFFER, base + (n_lights * 16) + i * 16,     sizeof(glm::vec3), light->GetRealColor());
        glBufferSubData(GL_UNIFORM_BUFFER, base + (n_lights * (16 + 16)) + i * 64,  sizeof(glm::mat4), &light->spaceMatrix);
    }
    base += n_lights * (16 + 16 + 64);
    for (unsigned int i = 0; i < scene->pointLights.size() && i < n_lights; i++) {
        light = scene->pointLights[i];
        glBufferSubData(GL_UNIFORM_BUFFER, base + i * 16,                   sizeof(glm::vec3), &light->worldPos);
        glBufferSubData(GL_UNIFORM_BUFFER, base + (n_lights * 16) + i * 16, sizeof(glm::vec3), light->GetRealColor());
    }
}

void Engine::RenderScene() {
    for (unsigned int i = 0; i < meshObjects.size(); i++) {
        RenderObject(meshObjects[i]);
    }
    RenderObject(scene->background);
}

void Engine::LoadObject2Buffers(SceneObject* object) {
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

    // Set Samplers
    shader->SetInt("_irrMap", unit);
    irrMap->Bind(unit);
    shader->SetInt("_prefilterMap", ++unit);
    prefilterMap->Bind(unit);
    shader->SetInt("_brdfLUT", ++unit);
    brdfLUTTexture->Bind(unit);
    for (unsigned int i = 0; i < scene->dirLights.size() && i < n_lights; i++) { 
        shader->SetInt("_shadowMap[" + std::to_string(i) + "]", ++unit);
        shadowMaps[i]->Bind(unit);
    }
    for (unsigned int i = 0; i < scene->pointLights.size() && i < n_lights; i++) { 
        shader->SetInt("_shadowCubeMap[" + std::to_string(i) + "]", ++unit);
        shadowCubeMaps[i]->Bind(unit);
    }

    // Update shader matrices to apply _transformations
    shader->SetMatrix("_transform", object->transform);
    
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

void Engine::ShadowSetup() {
    shadowShader = new Shader();
    shadowShader->Initialize("shaders/shadow_cast.vs", "shaders/shadow_cast.fs");
    shadowPointShader = new Shader();
    shadowPointShader->Initialize("shaders/point_shadow_cast.vs", "shaders/point_shadow_cast.fs");
    
    for (unsigned int i = 0; i < n_lights; i++) {
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

    for (unsigned int i = 0; i < n_lights; i++) {
        glGenFramebuffers(1, &depthCubeMapsFBO[i]);

        Texture* depthMap = new Texture();
        depthMap->DefaultTexture(2048, 2048, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32);
        depthMaps.push_back(depthMap);

        TextureCube* shadowCubeMap = new TextureCube();
        shadowCubeMap->DefaultTextureCube(2048, 2048, GL_RED, GL_R32F);
        shadowCubeMaps.push_back(shadowCubeMap);

        glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapsFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap->ID, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);  
    }
}

void Engine::ShadowCapture() { 
    glCullFace(GL_FRONT);

    LightObject *light;
    glm::mat4 lightProjection, lightView, lightSpaceMatrix, shadowProjection;
    glm::mat4 shadowTransforms[6];
    lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -15.0f, 20.0f);

    for (int i = 0; i < scene->dirLights.size() && i < n_lights; i++) {
        light = scene->dirLights[i];

        lightView = glm::lookAt(light->direction * 5.0f, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        light->spaceMatrix = lightSpaceMatrix;

        glViewport(0, 0, shadowMaps[i]->width, shadowMaps[i]->height);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapsFBO[i]);
        glClear(GL_DEPTH_BUFFER_BIT);

        shadowShader->Use();
        shadowShader->SetMatrix("_lightSpaceMatrix", lightSpaceMatrix);
        for (unsigned int j = 0; j < meshObjects.size(); j++) {
            if (meshObjects[j]->shadowCast) {
                shadowShader->SetMatrix("_transform", meshObjects[j]->transform);
                RenderMesh(meshObjects[j]->mesh);
            }
        }
    }

    glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
    for (int i = 0; i < scene->pointLights.size() && i < n_lights; i++) {
        light = scene->pointLights[i];

        shadowProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.5f, 25.0f);
        shadowTransforms[0] = shadowProjection * glm::lookAt(light->worldPos, light->worldPos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0));
        shadowTransforms[1] = shadowProjection * glm::lookAt(light->worldPos, light->worldPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0));
        shadowTransforms[2] = shadowProjection * glm::lookAt(light->worldPos, light->worldPos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
        shadowTransforms[3] = shadowProjection * glm::lookAt(light->worldPos, light->worldPos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0));
        shadowTransforms[4] = shadowProjection * glm::lookAt(light->worldPos, light->worldPos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0));
        shadowTransforms[5] = shadowProjection * glm::lookAt(light->worldPos, light->worldPos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0));
        
        glViewport(0, 0, shadowCubeMaps[i]->width, shadowCubeMaps[i]->height);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, depthCubeMapsFBO[i]);

        for (unsigned int j = 0; j < 6; j++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, shadowCubeMaps[i]->ID, 0);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            shadowPointShader->Use();
            shadowPointShader->SetMatrix("_shadowTransform", shadowTransforms[j]);
            shadowPointShader->SetVector("_pointlight0_pos", light->worldPos);
            shadowPointShader->SetFloat("_farPlane", 25.0f);
            for (unsigned int k = 0; k < meshObjects.size(); k++) {
                if (meshObjects[k]->shadowCast) {
                    shadowPointShader->SetMatrix("_transform", meshObjects[k]->transform);
                    RenderMesh(meshObjects[k]->mesh);
                }
            }
        } 
        
    }

    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::PBRcapture() {
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  

    irrMap = new TextureCube();
    irrMap->DefaultTextureCube(64, 64, GL_RGB, GL_RGB);

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
    prefilterMap->DefaultTextureCube(256, 256, GL_RGB, GL_RGB);

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
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // resize buffer
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
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // resize buffer
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    shader->SetMatrix("_projection", captureProjection);
    for (unsigned int i = 0; i < 6; i++) {
        shader->SetMatrix("_view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, target->ID, mipLevel);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
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
