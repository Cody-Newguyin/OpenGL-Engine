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

    // render settings
    bool wireframe = false;
    bool debug = true;
    const int n_lights = 4; 
    
    // PBR samples
    TextureCube* irrMap;
    TextureCube* prefilterMap;
    Texture* brdfLUTTexture;

    // Shadow samples
    const int n_cascades = 3;
    std::vector<float> planes{-5.0f, 5.0f, 20.0f, 40.0f};
    Shader* shadowShader;
    Shader* shadowCascadeShader;
    Shader* shadowPointShader;
    Shader* debugShader;
    unsigned int depthMapsFBO[4];
    std::vector<Texture*> shadowMaps;
    unsigned int ALTdepthMapsFBO[4];
    std::vector<Texture*> ALTshadowMaps;
    unsigned int depthCubeMapsFBO[4];
    std::vector<Texture*> depthMaps;
    std::vector<TextureCube*> shadowCubeMaps;


    public:
    Engine();
    void Initialize(GLFWwindow* window, Scene* scene);
    void Render();
   

    private:
    unsigned int captureFBO;
    unsigned int captureRBO;
    unsigned int globalUBO;
    std::vector<SceneObject*> meshObjects;
    
    private:
    void UpdateGlobalUniforms();
    void RenderScene();
    void LoadObject2Buffers(SceneObject* object);
    void ClearBuffers();
    void RenderObject(SceneObject* object);
    void RenderMesh(Mesh* mesh);
    glm::mat4 FitLight2Camera(glm::vec3 lightDir, float nearPlane, float farPlane);
    void ShadowSetup();
    void ShadowCapture();
    void PBRcapture();
    void Render2Texture(SceneObject* object, Texture* target);
    void Render2CubeMap(SceneObject* envCube, TextureCube* target, unsigned int mipLevel);
    void ResetViewport();
    void Debug();
    
};

#endif