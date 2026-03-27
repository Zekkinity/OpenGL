#include "ZkEngine/Utils/Timer.hpp"

#include "ZkEngine/Core/Log.hpp"

namespace zk {

Timer::Timer() {
    start = Clock::now();
}

Timer::~Timer() {
    auto end = Clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    ZK_CORE_INFO("Time: {} ms", duration);
}

// Timer::Timer(std::string_view name) {
//     start = Clock::now();
//     ZK_CORE_INFO("Timer: {}", name);
// }

} // namespace zk
