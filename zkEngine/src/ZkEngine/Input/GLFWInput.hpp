#pragma once

#include <GLFW/glfw3.h>

namespace zk::GLFWInput 
{
    void KeyCallback(GLFWwindow*, int key, int, int action, int);
    void MouseCallback(GLFWwindow*, double x, double y);
    void ScrollCallback(GLFWwindow*, double xOffset, double yOffset);

    void MouseButtonCallback(GLFWwindow*, int button, int action, int);
    void RegisterCallbacks(GLFWwindow* window);
} 
