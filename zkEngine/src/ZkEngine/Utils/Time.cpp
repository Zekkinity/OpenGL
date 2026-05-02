#include "zkpch.hpp"
#include "Time.hpp"

#include <GLFW/glfw3.h>

void Time::init() {
    lastTime = glfwGetTime();
}

void Time::update() {
    double currentTime = glfwGetTime();
    double unscaledDeltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Scale time
    deltaTime = unscaledDeltaTime * timeScale;
    accumulator += deltaTime;

    simulatedTime += deltaTime * timeScale;
}

bool Time::stepPhysics() {
    if (accumulator >= PHYSICS_DELTA_TIME) {
        accumulator -= PHYSICS_DELTA_TIME;
        return true;
    }
    return false;
}

double Time::getDeltaTime() {
    return deltaTime;
}

double Time::getTime() {
    return glfwGetTime();
}

double Time::getSimulatedTime() {
    return simulatedTime;
}

void Time::setTimeScale(double time) {
    timeScale = time;
}
