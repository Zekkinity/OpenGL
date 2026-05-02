#pragma once

#include <chrono>

namespace zk {

struct ProfileResult {
    std::string name;
    double ms;
};

using Clock = std::chrono::steady_clock;

class ScopedTimer {
  public:
    ScopedTimer(std::string_view name);
    ~ScopedTimer();

private:
    Clock::time_point m_Start;
    std::string m_Name;
};

} // namespace zk
