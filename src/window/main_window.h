#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <glad.h>
#include <glfw3.h>
#include <string>

class MainWindow {
    public:
    int width, height;
    std::string title;
    GLFWwindow* window;

    public:
    MainWindow();
    GLFWwindow* Initialize(int width, int height, std::string title);
    void Shutdown();
    void ProcessInput();

    private:
};

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

#endif