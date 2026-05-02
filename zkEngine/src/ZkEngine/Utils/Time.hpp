#pragma once

class Time {
public:
    static void init();
    static void update();

    static double getTime();
    static double getSimulatedTime();
    static double getDeltaTime();

    static bool stepPhysics();

    static void setTimeScale(double time);

private:
    static inline double lastTime = 0.0;
    static inline double accumulator = 0.0f;
    static inline double timeScale = 1.0;
    static inline double deltaTime = 0.0;

    static inline double simulatedTime = 0.0;

    const static inline double PHYSICS_DELTA_TIME = 1.0 / 60.0;
};
