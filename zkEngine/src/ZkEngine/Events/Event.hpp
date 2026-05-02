#pragma once

#include <cstdint>

namespace zk {

enum class EventType : uint16_t {
    WindowResize,
    WindowClose,
    KeyPressed,
    KeyReleased,
    MouseMoved,
    MouseButtonPressed,
    CursorLockChanged,
};

struct Event {
    EventType type;
    union {
        struct { int width, height; } resize;
        struct { int keycode; }       key;
        struct { float x, y; }        mouse;
        struct { bool locked; }       cursorLock;
    };
};

} // namespace zk
