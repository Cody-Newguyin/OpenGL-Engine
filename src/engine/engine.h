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

    // PBR samples
    TextureCube* irrMap;
    TextureCube* prefilterMap;
    Texture* brdfLUTTexture;

    // Shadow samples
    Shader* shadowShader;
    Shader* shadowPointShader;
    unsigned int depthMapsFBO[4];
    std::vector<Texture*> shadowMaps;
    unsigned int depthCubeMapsFBO[4];
    std::vector<TextureCube*> shadowCubeMaps;

    // render settings
    bool wireframe = false;
    bool debug = true;

    public:
    Engine();
    void Initialize(GLFWwindow* window, Scene* scene);
    void Render();
    void ShadowCapture();

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
    
    void PBRcapture();
    void Render2Texture(SceneObject* object, Texture* target);
    void Render2CubeMap(SceneObject* envCube, TextureCube* target, unsigned int mipLevel);
    void ResetViewport();
    
};

#endif