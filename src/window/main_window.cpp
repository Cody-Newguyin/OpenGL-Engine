#include "window/main_window.h"
#include "log/log.h"

MainWindow::MainWindow() {

}

GLFWwindow* MainWindow::Initialize(int width, int height, std::string title) {
    this->width = width;
    this->height = height;
    this->title = title;

    // Initalise glfw
    glfwInit();

    // Configure glfw requirements
    // Fails for OpenGl version < 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Set core-profile to ignore backwards-compatiablity 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create glfw window
    this->window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (window == NULL) {
        LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return NULL;
    }

    // Set opengl context
    glfwMakeContextCurrent(window);

    // Set callback
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // Load function pointers for opengl with glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_ERROR("Failed to initialize GLAD");
        return NULL;
    }

    return window;
}

void MainWindow::Shutdown() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

// Checks for input on a window
void MainWindow::ProcessInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Called whenever the window or framebuffer's size is changed
void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
