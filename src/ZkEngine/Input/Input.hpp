#pragma once

#include "KeyCodes.hpp"

using Key = Input::Key;
using MouseButton = Input::MouseButton;

namespace Input
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

    void SetKey(Key key, bool down);
    void SetMouseButton(MouseButton button, bool down);
    void SetMouse(float x, float y);
}

