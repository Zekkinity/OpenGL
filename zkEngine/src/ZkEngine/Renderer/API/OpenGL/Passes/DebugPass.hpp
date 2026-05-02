// #pragma once
//
// #include "ZkEngine/Renderer/Core/RenderPass.hpp"
//
// namespace zk {
//
// class OpenGLRenderer;
// class RenderView;
// class CameraData;
// class Shader;
//
// class DebugPass : public RenderPass {
//   public:
//     DebugPass(OpenGLRenderer* renderer);
//
//     void execute(const RenderContext& ctx, const RenderView& view) override;
//     void debug() override;
//
//     void drawLine(const std::vector<glm::vec3>& pts);
//
//   private:
//     void drawCameraFrustum(const CameraData& camera) const;
//
//   private:
//     OpenGLRenderer* m_Renderer;
//
//     Ref<Shader> m_DebugNormals;
//     Ref<Shader> m_LineShader;
//
//     bool m_NormalsEnabled = false;
//     bool m_FrustumEnabled = false;
//
//     uint32_t m_LineVAO = 0, m_LineVBO = 0;
//     struct LineStripCmd {
//         std::vector<glm::vec3> points;
//     };
//
//     std::vector<LineStripCmd> m_LineQueue;
// };
//
// } // namespace zk
