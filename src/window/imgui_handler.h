#ifndef _IMGUI_HANDLER_H_
#define _IMGUI_HANDLER_H_

#include <glad.h>
#include <glfw3.h>
#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "engine/engine.h"
#include "scene/scene_object.h"
#include "shader/basic_material.h"

class ImGuiHandler {
    public:
    GLFWwindow *window;
    Engine *engine;
    BasicMaterial *material;

    public:
    ImGuiHandler();
    void Initialize(GLFWwindow* window, Engine *engine);
    void Newframe();
    void Update();
    void Render();
    void ProcessInput();
    void Shutdown();

    private:
    void ShowObjectTreeNode(SceneObject* object, unsigned int id);
};


#endif