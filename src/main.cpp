#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "log/log.h"
#include "spdlog.h"

#include "window/main_window.h"
#include "window/imgui_handler.h"

#include "mesh/cube.h"
#include "mesh/sphere.h"
#include "mesh/plane.h"
#include "mesh/quad.h"
#include "scene/scene.h"
#include "scene/scene_object.h"
#include "scene/light/light_object.h"
#include "scene/background.h"
#include "shader/material.h"
#include "shader/shader.h"
#include "shader/texture.h"
#include "shader/texture_cube.h"
#include "shader/basic_material.h"
#include "utility/object_loader.h"
#include "utility/utility.h"

int main(int, char**){
    Log::Init();
    std::cout << "MyOpenGLEngine Started!" << std::endl;
    
    MainWindow mainWindow = MainWindow();
    GLFWwindow* window = mainWindow.Initialize(800, 600, "MyOpenGLEngine");
    if (!window) {
        std::cout << "MyOpenGLEngine Failed!" << std::endl;
        return -1;
    }
    Scene scene = Scene();
    Engine engine = Engine();
    engine.Initialize(window, &scene);
    
    // Load Shader
    Shader lightShader = Shader();
    lightShader.Initialize("shaders/light.vs", "shaders/light.fs");
    // Load Material
    Material lightMaterial = Material();
    lightMaterial.SetShader(&lightShader);

    // Load basic material
    BasicMaterial testMaterial = BasicMaterial("textures/texture.png");
    BasicMaterial marbleMaterial = BasicMaterial("textures/texture.png", "textures/heights.png", NORM_MAP_BUMP);
    
    // Load meshes
    Plane plane = Plane(2, 2);
    // Cube cube = Cube();
    Sphere sphere = Sphere(15, 15);
    // Quad quad = Quad();

    // Load objects into scene
    // SceneObject* bunnyObject = ObjectLoader::ReadObjFile("meshes/backpack/backpack.obj");
    // bunnyObject->SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
    // bunnyObject->SetName("Bunny");
    // scene.AddObject(bunnyObject);
    
    // SceneObject cubeObject = SceneObject(&cube, &marbleMaterial);
    // cubeObject.SetPosition(glm::vec3(2.0f, -2.0f, 0.0f));
    // scene.AddObject(&cubeObject);
    // cubeObject.SetName("Cube");

    SceneObject sphereObject = SceneObject(&sphere, &marbleMaterial);
    sphereObject.SetPosition(glm::vec3(-2.0f, -2.0f, 0.0f));
    scene.AddObject(&sphereObject);
    sphereObject.SetName("Sphere");

    SceneObject planeObject = SceneObject(&plane, &marbleMaterial);
    planeObject.SetPosition(glm::vec3(0.0f, -3.0f, 0.0f));
    planeObject.SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    planeObject.SetScale(glm::vec3(6.0f, 6.0f, 1.0f));
    planeObject.UpdateTransform();
    scene.AddObject(&planeObject);
    planeObject.SetName("Plane");

    // for (unsigned int i = 0; i < sphere.vertices.size(); i++) {
    //     // LOG_INFO("normal:");
    //     // LOG_GLM(plane.vertices[i].normal);
    //     LOG_INFO("tangent:");
    //     LOG_GLM(sphere.vertices[i].tangent);
    //     // LOG_INFO("transpose inverse normal:");
    //     // glm::mat3 transform = glm::mat3(glm::transpose(glm::inverse(sphereObject.transform)));
    //     // glm::vec3 transformedNormal = transform * plane.vertices[i].normal;
    //     // LOG_GLM(transformedNormal);
    //     LOG_INFO("transformed tangent:");
    //     glm::vec3 transformedTan = glm::normalize(glm::mat3(sphereObject.transform) * glm::vec3(sphere.vertices[i].tangent));
    //     LOG_GLM(transformedTan);
    // }

    // SceneObject quadObject = SceneObject(&quad, &testMaterial);
    // quadObject.SetPosition(glm::vec3(0.0f, 0.0f, -5.0f));
    // scene.AddObject(&quadObject);
    // quadObject.SetName("Quad 1");

    // Empty parent to rotate about
    SceneObject emptyObject = SceneObject();
    scene.AddObject(&emptyObject);
    emptyObject.SetName("Empty");
    
    // Create lights
    LightObject dirLight0 = LightObject(LIGHT_TYPE_DIR);
    dirLight0.rotation = glm::vec3(-10.0f, 60.0f, 0.0f);
    dirLight0.SetName("DirLight 1");
    scene.AddLight(&dirLight0);
    scene.AddObject(&dirLight0);

    LightObject pointLight0 = LightObject(LIGHT_TYPE_POINT);
    pointLight0.position = glm::vec3(1.0f, 1.0f, 0.0f);
    pointLight0.SetMesh(&sphere);
    pointLight0.SetMaterial(&lightMaterial);
    pointLight0.SetName("PointLight 1");
    scene.AddLight(&pointLight0);
    emptyObject.AddObject(&pointLight0);

    // Setup GUI
    ImGuiHandler guiHandler = ImGuiHandler();
    guiHandler.Initialize(window, &engine);
    guiHandler.material = &marbleMaterial;

    // main loop
    while(!glfwWindowShouldClose(window)) {
        // Polls os-events (inputs), update window state, callback functions
        glfwPollEvents();
        mainWindow.ProcessInput();
        guiHandler.ProcessInput();

        // Display GUI window
        guiHandler.Newframe();
        guiHandler.Update();

        // Update scene here
        // cubeObject.SetRotation((float)glfwGetTime() * glm::vec3(50.0f, 25.0f, 0.0f));
        emptyObject.SetRotation((float)glfwGetTime() * glm::vec3(0.0f, 50.0f, 0.0f));
        scene.root->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));

        // Render
        engine.Render();
        guiHandler.Render();

        // swap double buffers
        // front buffer holds output image, while draw commands write to back buffer
        glfwSwapBuffers(window);
    }
    scene.Clear();
    lightShader.Unload();
    guiHandler.Shutdown();
    mainWindow.Shutdown();
   
    std::cout << "MyOpenGLEngine Ended!" << std::endl;
    return 0;
}