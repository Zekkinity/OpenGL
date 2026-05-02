#include "zkpch.hpp"

#include "ZkEngine/Utils/Profiler.hpp"

#include "ZkEngine/Utils/Time.hpp"
#include "imgui.h"

namespace zk {

void Profiler::submit(ProfileResult result) {
    auto& stat = s_Stats[result.name];
    stat.totalMs += result.ms;
    stat.samples++;
}

void Profiler::init() {
    TracyGpuContext;
}

void Profiler::endFrame() {
    TracyGpuCollect;

    double dt = Time::getDeltaTime();
    s_Accumulator += dt;

    if (s_Accumulator >= s_Interval) {
        s_Accumulator = 0.0;

        s_Averages.clear();

        for (const auto& [name, stat] : s_Stats) {
            if (stat.samples == 0)
                continue;

            double avg = stat.totalMs / stat.samples;
            s_Averages[name] = avg;
        }

        s_Stats.clear();
    }
}

void Profiler::showStats() {
    ImGui::Begin("Profiler");

    double frameMS = 0.0;
    if (s_Averages.contains("Frame")) {
        frameMS = s_Averages["Frame"];
    }

    std::vector<std::pair<std::string, double>> sortedAverages(
        s_Averages.begin(), s_Averages.end());
    std::sort(sortedAverages.begin(), sortedAverages.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    for (const auto& [name, ms] : sortedAverages) {
        if (name == "Frame") {
            ImGui::Text("%s: %.2f ms (%.2f FPS)", name.c_str(), ms, 1000.0 / ms);
            continue;
        }

        double pct = (frameMS > 0.0) ? (ms / frameMS * 100.0) : 0.0;

        ImGui::Text("%s: %.4f ms (%.2f%%)", name.c_str(), ms, pct);
    }

    ImGui::End();
}

} // namespace zk
