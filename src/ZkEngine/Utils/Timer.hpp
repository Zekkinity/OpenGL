#pragma once

#include <chrono>

namespace zk {

using Clock = std::chrono::high_resolution_clock;

class Timer {
  public:
    Timer();
    ~Timer();

private:
    Clock::time_point start;
};

} // namespace zk
