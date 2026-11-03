#pragma once

namespace zk {

class RenderDevice;
class Framebuffer;
class Shader;
class Texture;

class SSAOPass {
  public:
    SSAOPass(RenderDevice* device);
    ~SSAOPass();

    uint32_t execute(uint32_t position, uint32_t normal);

    void resize(uint32_t width, uint32_t height);
    void debug();

  private:
    RenderDevice* m_Device = nullptr;

    Scope<Framebuffer> m_SSAOFBO;
    Ref<Shader> m_SSAOShader;

    std::vector<glm::vec3> ssaoKernel;

    Ref<Texture> m_NoiseTexture;

    Scope<Framebuffer> m_SSAOBlurFBO;
    Ref<Shader> m_SSAOBlurShader;

    int m_SSAOKernelSize = 64; // 64 default
    float m_SSAORadius = 0.5f;
    float m_SSAOBias = 0.025f;
};

} // namespace zk
