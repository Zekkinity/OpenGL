#pragma once

#include "KeyCodes.hpp"

using Key = zk::Input::Key;
using MouseButton = zk::Input::MouseButton;

namespace zk::Input
{

    void Init();
    void Update();

    bool IsKeyDown(Key key);
    bool IsKeyPressed(Key key);   // this frame
    bool IsKeyReleased(Key key);  // this frame

    bool IsMousePressed(MouseButton button);
    bool isMouseDown(MouseButton button);
    bool IsMouseReleased(MouseButton button);

    float GetMouseX();
    float GetMouseY();

    float GetMouseScroll();

    void SetKey(Key key, bool down);
    void SetMouseButton(MouseButton button, bool down);
    void SetMouse(float x, float y);
    void SetMouseScroll(float yOffset);
}

