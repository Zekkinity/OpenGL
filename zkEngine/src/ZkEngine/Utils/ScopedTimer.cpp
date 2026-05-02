#include "zkpch.hpp"

#include "ZkEngine/Utils/ScopedTimer.hpp"

#include "ZkEngine/Utils/Profiler.hpp"

namespace zk {

ScopedTimer::ScopedTimer(std::string_view name) : m_Name(name) {
    m_Start = Clock::now();
}

ScopedTimer::~ScopedTimer() {
    auto end = Clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - m_Start).count();

    Profiler::submit({m_Name, ms});
}

} // namespace zk
