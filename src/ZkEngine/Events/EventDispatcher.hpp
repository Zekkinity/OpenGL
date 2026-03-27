#pragma once

#include "Event.hpp"

#include <functional>
#include <vector>
#include <unordered_map>

namespace zk {

using EventCallback = std::function<void(const Event&)>;

class EventDispatcher {
public:
    void subscribe(EventType type, EventCallback cb) {
        m_Listeners[type].push_back(std::move(cb));
    }

    void push(const Event& e) {
        m_Queue.push_back(e);
    }

    // Call once per frame in Engine::update()
    void flush() {
        for (auto& e : m_Queue) {
            auto it = m_Listeners.find(e.type);
            if (it != m_Listeners.end())
                for (auto& cb : it->second)
                    cb(e);
        }
        m_Queue.clear();
    }

private:
    std::vector<Event> m_Queue;
    std::unordered_map<EventType, std::vector<EventCallback>> m_Listeners;
};

} // namespace zk
