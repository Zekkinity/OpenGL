#pragma once

#include "ZkEngine/Renderer/Core/RenderPass.hpp"

namespace zk {

struct BloomMip {
    glm::vec2 size;
    glm::ivec2 intSize;
    uint32_t texture = 0;
};

class RenderDevice;
class Framebuffer;
class Shader;

class BloomPass {
  public:
    BloomPass(RenderDevice* device);
    ~BloomPass();

    uint32_t execute(uint32_t srcTexture);
    void debug();

  private:
    void renderDownsamples(uint32_t srcTexture);
    void renderUpsamples();

  private:
    RenderDevice* m_Device = nullptr;

    uint32_t m_BloomFBO;

    std::vector<BloomMip> m_MipChain;

    Ref<Shader> m_DownsampleShader;
    Ref<Shader> m_UpsampleShader;

    float m_FilterRadius = 0.001f;
};

} // namespace zk
