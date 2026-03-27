#pragma once

#include "ZkEngine/Renderer/RendererBackend.hpp"

#include "glm/fwd.hpp"
#include <filesystem>
#include <vector>

namespace zk {

// This is the API that the application will use
class Renderer {
  public:
    static void init();

    static void update();
    static void beginFrame();
    // static void submit(const RenderObject& obj, const Camera3D& camera);
    static void endFrame();

    static RenderAPI getAPI();

    static void setClearColor(const glm::vec4& color);
    static void debug();

    static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    static void drawLine(const std::vector<glm::vec3>& pts);

    static void setSkybox(const std::vector<std::filesystem::path>& tex);

    static std::shared_ptr<Mesh> createCube();
    static std::shared_ptr<Mesh> createSphere(
        float radius = 0.5f, int sectorCount = 64, int stackCount = 64);
    static std::shared_ptr<Mesh> createLine();
    static std::shared_ptr<Mesh> createQuad();
    static std::shared_ptr<Mesh> createTriangle();

    static std::shared_ptr<Mesh> createGrid(int halfSize, float spacing = 1.0f);

    static RenderDevice& getDevice();
};

} // namespace zk
