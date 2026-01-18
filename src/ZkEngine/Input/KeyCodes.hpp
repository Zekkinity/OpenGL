#pragma once

namespace Input
{
    enum class Key
    {
        Unknown = 0,

        A, B, C, D, E, F, G, P, I,
        W, S, R,
        SPACE, ESCAPE, Enter,
        SHIFT,

        MouseLeft,
        MouseRight,
        MouseMiddle,

        Count
    };

    enum class MouseButton
    {
        Unknown = -1,
        Left = 0,
        Right,
        Middle,
        Count
    };
}

