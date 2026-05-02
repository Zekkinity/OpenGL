#pragma once

#include "ZkEngine/Utils/ScopedTimer.hpp"

#include <tracy/Tracy.hpp>
#include <glad/glad.h>
#include <tracy/TracyOpenGL.hpp>

namespace zk {

struct AccumulatedStat {
    double totalMs = 0.0;
    int samples = 0;
};

class Profiler {
  public:
    static void init();
    static void submit(ProfileResult result);
    static void endFrame();
    static void showStats();

  private:
    inline static std::unordered_map<std::string, AccumulatedStat> s_Stats;

    inline static std::unordered_map<std::string, double> s_Averages;

    inline static double s_Accumulator = 0.0;
    inline static double s_Interval = 0.2;
};

#define ZK_PROFILE_SCOPE(name)                                                                     \
    ScopedTimer zk_profile_##__LINE__(name);                                                       \
    ZoneScopedN(name);                                                                             \
    TracyGpuZone(name)

} // namespace zk
