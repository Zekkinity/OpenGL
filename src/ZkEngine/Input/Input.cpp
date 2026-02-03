#include "Input.hpp"

#include <array>
#include <cstddef>

namespace zk::Input
{

    static std::array<bool, (size_t)Key::Count> s_Current;
    static std::array<bool, (size_t)Key::Count> s_Previous;

    static std::array<bool, (size_t)MouseButton::Count> s_MouseCurrent;
    static std::array<bool, (size_t)MouseButton::Count> s_MousePrevious;

    static float s_MouseX = 0.0f;
    static float s_MouseY = 0.0f;
    static float s_MouseScroll = 0.0f;

    void Init()
    {
        s_Current.fill(false);
        s_Previous.fill(false);

        s_MouseCurrent.fill(false);
        s_MousePrevious.fill(false);
    }

    void Update()
    {
        s_Previous = s_Current;
        s_MousePrevious = s_MouseCurrent;
    }

    bool IsKeyDown(Key key)
    {
        return s_Current[(size_t)key];
    }

    bool IsKeyPressed(Key key)
    {
        size_t i = (size_t)key;
        return s_Current[i] && !s_Previous[i];
    }

    bool IsKeyReleased(Key key)
    {
        size_t i = (size_t)key;
        return !s_Current[i] && s_Previous[i];
    }

    bool IsMousePressed(MouseButton button) {
        size_t i = (size_t)button;
        return s_MouseCurrent[i] && !s_MousePrevious[i];
    }

    bool IsMouseDown(MouseButton button) {
        size_t i = (size_t)button;
        return s_MouseCurrent[i];
    }

    bool IsMouseReleased(MouseButton button) {
        size_t i = (size_t)button;
        return !s_MouseCurrent[i] && s_MousePrevious[i];
    }

    float GetMouseX() { return s_MouseX; }
    float GetMouseY() { return s_MouseY; }

    float GetMouseScroll() { 
        float scroll = s_MouseScroll;
        s_MouseScroll = 0.0f;
        return scroll;
    }

    // --- internal setters (used by platform layer) ---
    void SetKey(Key key, bool down)
    {
        s_Current[(size_t)key] = down;
    }

    void SetMouseButton(MouseButton button, bool down) {
        s_MouseCurrent[(size_t)button] = down;
    }

    void SetMouse(float x, float y)
    {
        s_MouseX = x;
        s_MouseY = y;
    }

    void SetMouseScroll(float scroll)
    {
        s_MouseScroll = scroll;
    }
}
