#include <GLFW/glfw3.h>

#include "ZkEngine/Input/Input.hpp"
#include "ZkEngine/Input/KeyCodes.hpp"

namespace GLFWInput {
    static Input::Key TranslateKey(int glfwKey)
    {
        using namespace Input;

        switch (glfwKey)
        {
            case GLFW_KEY_W: return Key::W;
            case GLFW_KEY_A: return Key::A;
            case GLFW_KEY_S: return Key::S;
            case GLFW_KEY_D: return Key::D;
            case GLFW_KEY_R: return Key::R;
            case GLFW_KEY_P: return Key::P;
            case GLFW_KEY_I: return Key::I;
            case GLFW_KEY_SPACE: return Key::SPACE;
            case GLFW_KEY_LEFT_SHIFT: return Key::SHIFT;
            case GLFW_KEY_ESCAPE: return Key::ESCAPE;
        }

        return Key::Unknown;
    }

    static Input::MouseButton TranslateMouseButton(int glfwButton)
    {
        using namespace Input;

        switch (glfwButton)
        {
            case GLFW_MOUSE_BUTTON_LEFT: return MouseButton::Left;
            case GLFW_MOUSE_BUTTON_RIGHT: return MouseButton::Right;
            case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
        }

        return MouseButton::Unknown;
    }

    void KeyCallback(GLFWwindow*, int key, int, int action, int)
    {
        Input::Key engineKey = TranslateKey(key);

        if (engineKey == Input::Key::Unknown)
            return;

        if (action == GLFW_PRESS)
            Input::SetKey(engineKey, true);
        else if (action == GLFW_RELEASE)
            Input::SetKey(engineKey, false);
    }

    void MouseButtonCallback(GLFWwindow*, int button, int action, int)
    {
        Input::MouseButton engineButton = TranslateMouseButton(button);

        if (engineButton == Input::MouseButton::Unknown)
            return;

        if (action == GLFW_PRESS)
            Input::SetMouseButton(engineButton, true);
        else if (action == GLFW_RELEASE)
            Input::SetMouseButton(engineButton, false);
    }

    void MouseCallback(GLFWwindow*, double x, double y)
    {
        Input::SetMouse((float)x, (float)y);
    }

    void RegisterCallbacks(GLFWwindow* window)
    {
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        glfwSetCursorPosCallback(window, MouseCallback);
    }
}
