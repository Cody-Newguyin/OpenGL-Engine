#ifndef _IMGUI_HANDLER_H_
#define _IMGUI_HANDLER_H_

#include <glad.h>
#include <glfw3.h>
#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "renderer/renderer.h"
#include "scene/scene_object.h"
#include "shader/basic_material.h"

class ImGuiHandler {
    public:
    GLFWwindow *window;
    Renderer *renderer;
    std::vector<BasicMaterial*> materials;

    public:
    ImGuiHandler();
    void Initialize(GLFWwindow* window, Renderer *renderer);
    void Newframe();
    void Update();
    void Render();
    void ProcessInput();
    void Shutdown();

    private:
    void ShowObjectTreeNode(SceneObject* object, unsigned int id);
    void ShowMaterialTreeNode(BasicMaterial *material, unsigned int id);
};


#endif