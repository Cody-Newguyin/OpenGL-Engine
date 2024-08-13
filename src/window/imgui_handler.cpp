#include "window/imgui_handler.h"

ImGuiHandler::ImGuiHandler() {

}

void ImGuiHandler::Initialize(GLFWwindow *window, Engine *engine) {
    this->window = window;
    this->engine = engine;
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;      
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void ImGuiHandler::Newframe() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiHandler::Update() {
    Scene *scene = engine->scene;
    ImGuiIO& io = ImGui::GetIO();
    // ImGui::ShowDemoWindow();

    ImGui::Begin("Engine");

    ImGui::Checkbox("Wireframe", &engine->wireframe);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    
    ImGui::SeparatorText("Lights");
    for (unsigned int i = 0; i < scene->pointLights.size(); i++) {
        ImGui::ColorEdit3(scene->pointLights[i]->name.c_str(), (float*) &scene->pointLights[i]->color);
        ImGui::SliderFloat("Intensity_Point_" + i, &scene->pointLights[i]->intensity, 0.0f, 5.0f, "ratio = %.3f");
    }
    for (unsigned int i = 0; i < scene->dirLights.size(); i++) {
        ImGui::ColorEdit3(scene->dirLights[i]->name.c_str(), (float*) &scene->dirLights[i]->color);
        ImGui::SliderFloat("Intensity_Dir_" + i, &scene->dirLights[i]->intensity, 0.0f, 5.0f, "ratio = %.3f");
    }

    ImGui::SeparatorText("Materials");
    for (unsigned int i = 0; i < materials.size(); i++) {
        ShowMaterialTreeNode(materials[i], i);
    }

    ImGui::SeparatorText("Objects");
    for (unsigned int i = 0; i < scene->root->children.size(); i++) {
        ShowObjectTreeNode(scene->root->children[i], i);
    }

    ImGui::End();
}

void ImGuiHandler::ShowMaterialTreeNode(BasicMaterial *material, unsigned int id) {
    if (ImGui::TreeNode((void*)(intptr_t)id, material->name.c_str(), id)) {
        ImGui::ColorEdit3("", (float*) &material->color);
        if (material->smoothnessFile.empty()) ImGui::SliderFloat("Smoothness", &material->smoothness, 0.0f, 1.0f, "ratio = %.3f");
        if (material->metallicFile.empty()) ImGui::SliderFloat("Metallic", &material->metallic, 0.0f, 1.0f, "ratio = %.3f");
        if (material->ambientOcclusionFile.empty()) ImGui::SliderFloat("Ambient", &material->ambient, 0.0f, 1.0f, "ratio = %.3f");
        if (!material->normalFile.empty()) ImGui::DragFloat("Bump Scale", &material->bumpScale, 0.005f, 0.75f, 5.0f);
        ImGui::TreePop();
    }
}

void ImGuiHandler::ShowObjectTreeNode(SceneObject *object, unsigned int id) {
    if (ImGui::TreeNode((void*)(intptr_t)id, object->name.c_str(), id)) {
        ImGui::InputFloat3("position", (float*) &object->position);
        ImGui::InputFloat3("rotation", (float*) &object->rotation);
        ImGui::InputFloat3("scale", (float*) &object->scale);
        // Recursively check children
        for (unsigned int i = 0; i < object->children.size(); i++) {
            ShowObjectTreeNode(object->children[i], i);
        }
        ImGui::TreePop();
    }
}

void ImGuiHandler::Render() {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void ImGuiHandler::ProcessInput() {
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
        glfwSetWindowShouldClose(window, true);
    }
}

void ImGuiHandler::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
