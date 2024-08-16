#include "camera/camera.h"

bool firstMouse = true;

Camera::Camera() {

}

void Camera::Initialize() {
    width = 1280.0f;
    height = 960.0f;
    fov = 60.0f;
    nearPlane = 0.1f;
    farPlane = 100.0f;
    movementSpeed = 4.0f;
    MouseSensitivity = 0.1f;
    position = glm::vec3(0.0f, 0.0f, 7.0f);
    rotation = glm::vec3(0.0f, -90.0f, 0.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    UpdateCameraVectors();
}

void Camera::Update() {
    view = glm::lookAt(position, position + forward, up); 
    projection = glm::perspective(glm::radians(fov), width / height, nearPlane, farPlane);
}

void Camera::ProcessInput(GLFWwindow* window) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        movementSpeed = 4.0f;
    else
        movementSpeed = 2.0f;

    float velocity = movementSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += forward * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= forward * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * velocity;
    
    
    double xposIn, yposIn;
    glfwGetCursorPos(window, &xposIn, &yposIn);
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        rotation.y += xoffset;
        rotation.x += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (rotation.x > 89.0f)
            rotation.x = 89.0f;
        if (rotation.x < -89.0f)
            rotation.x = -89.0f;
    }
    lastX = xpos;
    lastY = ypos;

    this->dirty = true;
    UpdateCameraVectors();
    UpdateTransform();
}

void Camera::UpdateCameraVectors() { 
    forward.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
    forward.y = sin(glm::radians(rotation.x));
    forward.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
    forward = glm::normalize(forward);
    // // also re-calculate the Right and Up vector
    // // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));  
    up    = glm::normalize(glm::cross(right, forward));
}
