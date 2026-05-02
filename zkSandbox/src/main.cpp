#include "ZkEngine/Core/Engine.hpp"
#include "AppTest.hpp"

int main() {
    zk::EngineSpecs specs;
    specs.title = "OpenGL Engine";
    specs.width = 1280;
    specs.height = 720;
    specs.vsync = true;

    zk::Engine engine(specs);

    AppTest app;

    engine.run(app);

    return 0;
}
