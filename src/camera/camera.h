#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <scene/scene_object.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera : public SceneObject {
    public:
    // matrices to pass to shader
    glm::mat4 projection;
    glm::mat4 view;

    // transform information
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;

    // camera settings
    float width, height, fov, nearPlane, farPlane;
    // input settings
    float movementSpeed, MouseSensitivity;

    public:
    Camera();
    void Initialize();
    void Update();
    void ProcessInput(GLFWwindow* window);

    private:
    float deltaTime, lastFrame;
    float lastX, lastY;
    
    private:
    void UpdateCameraVectors();
};

#endif