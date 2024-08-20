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
#include "mesh/torus.h"
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
    GLFWwindow* window = mainWindow.Initialize(1280, 960, "MyOpenGLEngine");
    if (!window) {
        std::cout << "MyOpenGLEngine Failed!" << std::endl;
        return -1;
    }
    Scene scene = Scene();
    Renderer renderer = Renderer();
    renderer.Initialize(window, &scene);
    
    std::vector<BasicMaterial*> materials;

    // Load Shader
    Shader lightShader = Shader();
    lightShader.Initialize("shaders/light.vs", "shaders/light.fs");
    // Load Material
    Material lightMaterial = Material();
    lightMaterial.SetShader(&lightShader);

    // Load basic materials
    BasicMaterial defaultMat = BasicMaterial("Default");
    defaultMat.smoothness = 0.5f;
    defaultMat.metallic = 0.0f;
    defaultMat.Initalize();
    materials.push_back(&defaultMat);

    BasicMaterial circuitMat = BasicMaterial("Circuit");
    circuitMat.SetMainFile("textures/circuitry/circuitry-albedo.png");
    circuitMat.SetNormalFile("textures/circuitry/circuitry-normals-dented.png", NORM_MAP_NORM);
    circuitMat.SetMetallicFile("textures/circuitry/circuitry-metallic.png");
    circuitMat.SetSmoothnessFile("textures/circuitry/circuitry-smoothness.png", SMOOTH_MAP_SMOOTH);
    circuitMat.SetAmbientOcclustionFile("textures/circuitry/circuitry-occlusion.png");
    circuitMat.SetEmissionFile("textures/circuitry/circuitry-emission.png");
    circuitMat.Initalize();
    materials.push_back(&circuitMat);

    // Load meshes
    Plane plane = Plane(2, 2);
    Cube cube = Cube();
    Sphere sphere = Sphere(30, 30);
    Torus torus = Torus(2.0, 0.4, 30, 30);

    // Quad quad = Quad();

    // Setup object loader
    ObjectLoader objLoader = ObjectLoader();
    objLoader.defaultMat = &defaultMat;

    // Load objects into scene

    // objLoader.normType = NORM_MAP_NORM;
    // objLoader.smoothType = SMOOTH_MAP_ROUGH;
    // SceneObject* backpack = objLoader.ReadObjFile("meshes/backpack/backpack.obj", "Backpack", false);
    // scene.AddObject(backpack);

    BasicMaterial cerberusMat = BasicMaterial("Cerberus");
    cerberusMat.SetFlipImage(true);
    cerberusMat.SetMainFile("meshes/Cerberus_by_Andrew_Maximov/Textures/Cerberus_A.tga");
    cerberusMat.SetNormalFile("meshes/Cerberus_by_Andrew_Maximov/Textures/Cerberus_N.tga", NORM_MAP_NORM);
    cerberusMat.SetMetallicFile("meshes/Cerberus_by_Andrew_Maximov/Textures/Cerberus_M.tga");
    cerberusMat.SetSmoothnessFile("meshes/Cerberus_by_Andrew_Maximov/Textures/Cerberus_R.tga", SMOOTH_MAP_ROUGH);
    // cerberusMat.SetAmbientOcclustionFile("meshes/Cerberus_by_Andrew_Maximov/Raw/Cerberus_AO.tga");
    cerberusMat.Initalize();
    materials.push_back(&cerberusMat);

    objLoader.defaultMat = &cerberusMat;
    SceneObject* cerberus = objLoader.ReadObjFile("meshes/Cerberus_by_Andrew_Maximov/Cerberus_LP.obj", "Cerberus", true);\
    cerberus->SetScale(glm::vec3(0.05f, 0.05f, 0.05f));
    scene.AddObject(cerberus);

    // objLoader.normType = NORM_MAP_NORM;
    // objLoader.smoothType = SMOOTH_MAP_ROUGH;
    // SceneObject* sponza = objLoader.ReadObjFile("meshes/sponza/sponza.obj", "sponza",false, true);
    // sponza->SetScale(glm::vec3(0.005f, 0.005f, 0.005f));
    // sponza->SetName("Sponza");
    // scene.AddObject(sponza);

    SceneObject emptyObject = SceneObject("Empty");
    emptyObject.SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
    scene.AddObject(&emptyObject);

    SceneObject cubeObject = SceneObject(&cube, &circuitMat, "Cube");
    cubeObject.SetPosition(glm::vec3(2.0f, -2.0f, 0.0f));
    scene.AddObject(&cubeObject);

    SceneObject sphereObject = SceneObject(&sphere, &defaultMat, "Sphere");
    sphereObject.SetPosition(glm::vec3(-2.0f, -2.0f, 0.0f));
    scene.AddObject(&sphereObject);

    SceneObject planeObject = SceneObject(&plane, &defaultMat, "Plane");
    planeObject.SetPosition(glm::vec3(0.0f, -4.0f, 0.0f));
    planeObject.SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    planeObject.SetScale(glm::vec3(30.0f, 30.0f, 1.0f));
    scene.AddObject(&planeObject);
   
    
    // Create lights
    LightObject dirLight0 = LightObject(LIGHT_TYPE_DIR, "DirLight 1");
    dirLight0.rotation = glm::vec3(-10.0f, 60.0f, 0.0f);
    scene.AddLight(&dirLight0);
    scene.AddObject(&dirLight0);

    LightObject pointLight0 = LightObject(LIGHT_TYPE_POINT, "PointLight 1");
    pointLight0.SetPosition(glm::vec3(3.0f, 0.0f, 0.0f));
    pointLight0.SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
    pointLight0.SetMesh(&sphere);
    pointLight0.SetMaterial(&lightMaterial);
    scene.AddLight(&pointLight0);
    emptyObject.AddObject(&pointLight0);

    // Setup GUI
    ImGuiHandler guiHandler = ImGuiHandler();
    guiHandler.Initialize(window, &renderer);
    guiHandler.materials = materials;

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
        cubeObject.SetRotation((float)glfwGetTime() * glm::vec3(50.0f, 25.0f, 0.0f));
        sphereObject.SetRotation((float)glfwGetTime() * glm::vec3(50.0f, 25.0f, 0.0f));
        emptyObject.SetRotation((float)glfwGetTime() * glm::vec3(0.0f, 50.0f, 0.0f));
        
        // Render
        renderer.Render();
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


    // Sponze scene

    // objLoader.normType = NORM_MAP_NORM;
    // objLoader.smoothType = SMOOTH_MAP_ROUGH;
    // SceneObject* sponza = objLoader.ReadObjFile("meshes/sponza/sponza.obj", "sponza",false, true);
    // sponza->SetScale(glm::vec3(0.05f, 0.05f, 0.05f));
    // sponza->SetName("Sponza");
    // scene.AddObject(sponza);

    // get materials as well
    // materials.insert(materials.end(), objLoader.storedMats.begin(), objLoader.storedMats.end());
    
    // Empty parent to rotate about
    // SceneObject emptyObject = SceneObject("Empty");
    // emptyObject.SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
    // scene.AddObject(&emptyObject);

    // SceneObject torusObject1 = SceneObject(&torus, &defaultMat, "Torus");
    // torusObject1.SetScale(glm::vec3(0.3f, 0.3f, 0.3f));
    // torusObject1.SetRotation(glm::vec3(0.0f, -40.0f, 0.0f));
    // emptyObject.AddObject(&torusObject1);

    // SceneObject torusObject2 = SceneObject(&torus, &defaultMat, "Torus");
    // torusObject2.SetScale(glm::vec3(0.45f, 0.45f, 0.45f));
    // torusObject2.SetRotation(glm::vec3(45.0f, 45.0f, 0.0f));
    // emptyObject.AddObject(&torusObject2);
        
    // torusObject1.SetRotation((float)glfwGetTime() * glm::vec3(50.0f, 25.0f, 0.0f));
    // torusObject2.SetRotation((float)glfwGetTime() * glm::vec3(25.0f, 50.0f, 0.0f));
    // float displacement = sin((float)glfwGetTime()) * 2;
    // emptyObject.SetPosition(glm::vec3(displacement, 2.0f, -0.2f));
