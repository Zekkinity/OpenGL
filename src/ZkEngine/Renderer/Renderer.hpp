#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <memory>

#include "ZkEngine/Camera3D.hpp"
#include "ZkEngine/RenderObject.hpp"

class ModelObject;

class Renderer {
   public:
    void init();

    void update();

    void beginFrame() const;
    void submit(const RenderObject& obj, const Camera3D& camera);
    void endFrame();

    void debug();

   private:
    struct DrawCommand {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
        glm::mat4 model;
        glm::mat4 viewProj;
        glm::vec4 color;
        bool colorOverride;
    };

    std::vector<DrawCommand> m_DrawQueue;

    bool m_Debug = true;

    float m_ClearColor[4] = {0.2f, 0.3f, 0.8f, 1.0f};
    bool m_WireFrame = false;
};
