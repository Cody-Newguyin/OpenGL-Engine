#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <glad.h>
#include <glfw3.h>
#include <vector>

#include "camera/camera.h"
#include "mesh/mesh.h"
#include "mesh/sphere.h"
#include "scene/scene.h"
#include "scene/scene_object.h"
#include "scene/background.h"
#include "scene/light/light_object.h"
#include "shader/shader.h"
#include "shader/texture_cube.h"

class Engine {
    public:
    GLFWwindow* window;
    Scene* scene;
    Camera camera;

    // PBR Samples
    TextureCube* irrMap;
    TextureCube* prefilterMap;
    Texture* brdfLUTTexture;

    // Shadow shit
    Shader* shadowShader;
    unsigned int depthMapsFBO[4];
    std::vector<Texture*> shadowMaps;

    // render settings
    bool wireframe = false;
    bool debug = true;

    public:
    Engine();
    void Initialize(GLFWwindow* window, Scene* scene);
    void Render();
    
    private:
    unsigned int captureFBO;
    unsigned int captureRBO;
    std::vector<SceneObject*> meshObjects;
    

    private:
    void UpdateGlobalUniforms(Shader* shader);
    void RenderScene();
    void LoadObject2Buffers(SceneObject* object);
    void ClearBuffers();
    void RenderObject(SceneObject* object);
    void RenderMesh(Mesh* mesh);
    void ShadowCapture();
    void PBRcapture();
    void Render2Texture(SceneObject* object, Texture* target);
    void Render2CubeMap(SceneObject* envCube, TextureCube* target, unsigned int mipLevel);
    void ResetViewport();
    
};

#endif