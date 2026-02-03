#pragma once

namespace zk::Input
{
    enum class Key
    {
        Unknown = 0,

        A, B, C, D, E, F, G, H, I, J, K, L, M, N, P, O, V,
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

