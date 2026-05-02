// #pragma once
//
// #include "ZkEngine/Renderer/API/OpenGL/InstanceBuffer.hpp"
// #include "ZkEngine/Renderer/Core/RenderPass.hpp"
//
// namespace zk {
//
// class Mesh;
// class Material;
// class RenderCommand;
//
// class OpenGLRenderer;
// class RenderQueue;
// class Shader;
// struct LightData;
//
// class GeometryPass : public RenderPass {
//   public:
//     GeometryPass(OpenGLRenderer* renderer);
//
//     void execute(const RenderContext& ctx, const RenderView& view) override;
//     void debug() override;
//
//   private:
//     void draw(const RenderCommand& cmd);
//     void uploadLightsToShader(Shader& shader, const LightData& data);
//
//   private:
//     OpenGLRenderer* m_Renderer;
//
//     bool m_EnableTextures = true;
//     bool m_EnableBlinn = true;
//     bool m_EnableMapping = true;
//     bool m_Wireframe = false;
//
//     // Material shininess
//     float m_Shininess = 64.0f;
// };
//
// } // namespace zk
