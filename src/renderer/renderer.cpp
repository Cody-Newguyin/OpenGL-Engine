#include "renderer/renderer.h"
#include "log/log.h"
#include "mesh/cube.h"
#include "mesh/quad.h"
#include <cfloat>

Renderer::Renderer() {
    camera = Camera();
}

void Renderer::Initialize(GLFWwindow* window, Scene* scene) {
    this->window = window;
    this->scene = scene;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);

    camera.Initialize();

    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  
    scene->background = new Background();
    pbr = new PBR(this);
    ShadowSetup();

    // Post processing stuff
    quad = new Quad();
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    FBTexture = new Texture();
    FBTexture->DefaultTexture(scrWidth, scrHeight, GL_RGB, GL_RGB);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBTexture->ID, 0);

    glGenRenderbuffers(1, & RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, scrWidth, scrHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
    postShader = new Shader();
    postShader->Initialize("shaders/screenquad.vs", "shaders/post.fs");
    postShader->SetInt("_screen", 0);

    // Setup Uniform buffer object
    glGenBuffers(1, &globalUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, globalUBO);
    glBufferData(GL_UNIFORM_BUFFER, 2048, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalUBO);
}

void Renderer::Render() {
    // Geometry buffer
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

    // update camera
    camera.ProcessInput(window);
    camera.Update();

    scene->Update();
    LoadObject2Buffers(scene->root);

    ShadowCapture();

    ResetViewport();

    UpdateGlobalUniforms();

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderScene();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    postShader->Use();
    FBTexture->Bind(0);
    RenderMesh(quad);

    ClearBuffers();
}

void Renderer::UpdateGlobalUniforms() {
    glBindBuffer(GL_UNIFORM_BUFFER, globalUBO);
    glBufferSubData(GL_UNIFORM_BUFFER,   0, sizeof(glm::mat4), &camera.projection); 
    glBufferSubData(GL_UNIFORM_BUFFER,  64, sizeof(glm::mat4), &camera.view); 
    glBufferSubData(GL_UNIFORM_BUFFER, 128, sizeof(glm::vec3), &camera.position);
    
    int base = 144;
    for (unsigned int i =  0; i < (n_cascades + 1); i++) {
        glBufferSubData(GL_UNIFORM_BUFFER, base + i * 16, sizeof(float), &planes[i]);
    }
    

    // update light information
    LightObject *light;
    base += 16 * (n_cascades + 1);
    for (unsigned int i = 0; i < scene->dirLights.size() && i < n_lights; i++) {
        light = scene->dirLights[i];
        glBufferSubData(GL_UNIFORM_BUFFER, base + i * 16,                       sizeof(glm::vec3), &light->direction);
        glBufferSubData(GL_UNIFORM_BUFFER, base + (n_lights * 16) + i * 16,     sizeof(glm::vec3), light->GetRealColor());
        for (unsigned int j = 0; j < n_cascades; j++) {
            glBufferSubData(GL_UNIFORM_BUFFER, base + (n_lights * (16 + 16)) + (i + j) * 64,  sizeof(glm::mat4), &light->spaceMatrices[j]);
        }
    }
    base += n_lights * (16 + 16 + 64 * n_cascades);
    for (unsigned int i = 0; i < scene->pointLights.size() && i < n_lights; i++) {
        light = scene->pointLights[i];
        glBufferSubData(GL_UNIFORM_BUFFER, base + i * 16,                   sizeof(glm::vec3), &light->worldPos);
        glBufferSubData(GL_UNIFORM_BUFFER, base + (n_lights * 16) + i * 16, sizeof(glm::vec3), light->GetRealColor());
    }
}

void Renderer::RenderScene() {
    for (unsigned int i = 0; i < meshObjects.size(); i++) {
        RenderObject(meshObjects[i]);
    }
    RenderObject(scene->background);
}

void Renderer::LoadObject2Buffers(SceneObject* object) {
    // check compenets and add em
    if (object->mesh) {
        meshObjects.push_back(object);
    }

    // Recursively check children
    for (unsigned int i = 0; i < object->children.size(); i++) {
        LoadObject2Buffers(object->children[i]);
    }
}

void Renderer::ClearBuffers() {
    meshObjects.clear();
}

void Renderer::RenderObject(SceneObject* object) {
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
    pbr->irrMap->Bind(unit);
    shader->SetInt("_prefilterMap", ++unit);
    pbr->prefilterMap->Bind(unit);
    shader->SetInt("_brdfLUT", ++unit);
    pbr->brdfLUTTexture->Bind(unit);
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
    
    // Finally draw the mesh
    RenderMesh(mesh);
}

void Renderer::RenderMesh(Mesh* mesh) {
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

void Renderer::ShadowSetup() {
    shadowCascadeShader = new Shader();
    shadowCascadeShader->Initialize("shaders/shadow_cast_copy.vs", "shaders/shadow_cast.fs", "shaders/shadow_cast.gs");
    shadowPointShader = new Shader();
    shadowPointShader->Initialize("shaders/point_shadow_cast.vs", "shaders/point_shadow_cast.fs");

    for (unsigned int i = 0; i < n_lights; i++) {
        // Genereate alt buffers 
        glGenFramebuffers(1, &depthMapsFBO[i]);

        Texture* shadowMap = new Texture();
        shadowMap->DefaultTextureArray(2048, 2048, n_cascades, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
        shadowMaps.push_back(shadowMap);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapsFBO[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap->ID, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Generate buffers for point light shadows
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

glm::mat4 Renderer::FitLight2Camera(glm::vec3 lightDir, float nearPlane, float farPlane) {
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane - 1.0f, farPlane + 1.0f);
    glm::mat4 frustum = glm::inverse(projection * camera.view);

    glm::vec4 corners[8];
    glm::vec4 boundingVertices[8] = {
        {-1.0f,	-1.0f,	-1.0f,	1.0f},
        {-1.0f,	-1.0f,	1.0f,	1.0f},
        {-1.0f,	1.0f,	-1.0f,	1.0f},
        {-1.0f,	1.0f,	1.0f,	1.0f},
        {1.0f,	-1.0f,	-1.0f,	1.0f},
        {1.0f,	-1.0f,	1.0f,	1.0f},
        {1.0f,	1.0f,	-1.0f,	1.0f},
        {1.0f,	1.0f,	1.0f,	1.0f}
    };
    for (unsigned int i = 0; i < 8; i++) {
        corners[i] = frustum * boundingVertices[i];
        corners[i] /= corners[i].w;
    }
    glm::vec3 center = glm::vec3(0, 0, 0);
    for (unsigned int i = 0; i < 8; i++) {
        center += glm::vec3(corners[i]);
    }
    center /= 8;

    glm::mat4 lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0, 1.0, 0.0));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (unsigned int i = 0; i < 8; i++)
    {
        const auto trf = lightView * corners[i];
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    float zMult = 2.0f;
    if (minZ < 0) {
        minZ *= zMult;
    } else {
        minZ /= zMult;
    }
    if (maxZ < 0) {
        maxZ /= zMult;
    } else {
        maxZ *= zMult;
    }

    glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    // lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -15.0f, 20.0f);
    return lightProjection * lightView;
}

void Renderer::ShadowCapture() { 
    glCullFace(GL_FRONT);

    LightObject *light;
    glm::mat4 shadowProjection;
    glm::mat4 lightSpaceMatrices[n_cascades];
    glm::mat4 shadowTransforms[6];

    for (int i = 0; i < scene->dirLights.size() && i < n_lights; i++) {
        light = scene->dirLights[i];

        glViewport(0, 0, shadowMaps[i]->width, shadowMaps[i]->height);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapsFBO[i]);
        glClear(GL_DEPTH_BUFFER_BIT);

        shadowCascadeShader->Use();
        for (unsigned int j = 0; j < n_cascades; j++) {
            lightSpaceMatrices[j] = FitLight2Camera(light->direction, planes[j], planes[j + 1]);
            light->spaceMatrices[j] = lightSpaceMatrices[j];
            shadowCascadeShader->SetMatrix("_lightSpaceMatrices[" + std::to_string(j) + "]", lightSpaceMatrices[j]);
        }
        for (unsigned int j = 0; j < meshObjects.size(); j++) {
            if (meshObjects[j]->shadowCast) {
                shadowCascadeShader->SetMatrix("_transform", meshObjects[j]->transform);
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

void Renderer::Render2Texture(SceneObject* object, Texture* target) {
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

void Renderer::Render2CubeMap(SceneObject* envCube, TextureCube* target, unsigned int mipLevel) {
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
    glm::mat4 captureViews[] = {
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

void Renderer::ResetViewport() {
    glViewport(0, 0, scrWidth, scrHeight);
}
