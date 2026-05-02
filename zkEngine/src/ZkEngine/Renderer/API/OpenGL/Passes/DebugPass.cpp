// #include "zkpch.hpp"
// #include "DebugPass.hpp"
//
// #include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"
//
// #include "ZkEngine/Renderer/Core/RenderQueue.hpp"
// #include "ZkEngine/Renderer/Core/RenderView.hpp"
// #include "ZkEngine/World/Entity.hpp"
// #include "ZkEngine/Renderer/Core/RenderContext.hpp"
// #include "ZkEngine/Core/AppAcess.hpp"
// #include "ZkEngine/Renderer/Shader.hpp"
//
// #include "imgui.h"
//
// namespace zk {
//
// DebugPass::DebugPass(OpenGLRenderer* renderer) : m_Renderer(renderer) {
//     m_DebugNormals = Asset::loadShader(
//         "shaders/VisualizeNormals.vert",
//         "shaders/VisualizeNormals.frag",
//         "shaders/VisualizeNormals.geom");
//     m_LineShader = Asset::loadShader("shaders/line.vert", "shaders/line.frag");
//
//     // Line stuff
//     glGenVertexArrays(1, &m_LineVAO);
//     glGenBuffers(1, &m_LineVBO);
//
//     glBindVertexArray(m_LineVAO);
//     glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
//
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
//
//     glBindVertexArray(0);
// }
//
// void DebugPass::execute(const RenderContext& ctx, const RenderView& view) {
//     if (m_FrustumEnabled) {
//         for (const auto& camera : ctx.world->getCameras()) {
//             // if (camera == ctx.world->getActiveCameraID())
//             //     continue;
//
//             auto cameraData = ctx.world->getCameraData(camera);
//             drawCameraFrustum(cameraData);
//         }
//     }
//
//     if (m_NormalsEnabled) {
//         m_DebugNormals->bind();
//         // glDepthMask(GL_FALSE);
//
//         for (const auto& cmd : ctx.queue->getOpaque()) {
//             m_DebugNormals->setMat4("u_Model", cmd.modelMatrix);
//
//             m_Renderer->drawMesh(cmd.mesh);
//         }
//
//         for (const auto& cmd : ctx.queue->getTransparent()) {
//             m_DebugNormals->setMat4("u_Model", cmd.modelMatrix);
//
//             m_Renderer->drawMesh(cmd.mesh);
//         }
//
//         // glDepthMask(GL_TRUE);
//     }
//
//     if (m_LineQueue.empty())
//         return;
//
//     // Draw lines
//     m_LineShader->bind();
//     m_LineShader->setVec3("u_Color", glm::vec3(1, 1, 1));
//
//     glDisable(GL_CULL_FACE);
//     glBindVertexArray(m_LineVAO);
//     for (const auto& cmd : m_LineQueue) {
//         glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
//
//         glBufferData(
//             GL_ARRAY_BUFFER,
//             cmd.points.size() * sizeof(glm::vec3),
//             cmd.points.data(),
//             GL_DYNAMIC_DRAW);
//
//         m_Renderer->drawArrays(GL_LINES, cmd.points.size());
//     }
//     glEnable(GL_CULL_FACE);
//
//     m_LineQueue.clear();
// }
//
// void DebugPass::drawLine(const std::vector<glm::vec3>& pts) {
//     m_LineQueue.emplace_back(LineStripCmd{pts});
// }
//
// void DebugPass::debug() {
//     ImGui::Checkbox("Enable Normals", &m_NormalsEnabled);
//     ImGui::Checkbox("Enable Frustum", &m_FrustumEnabled);
// }
//
// void DebugPass::drawCameraFrustum(const CameraData& camera) const {
//     // NDC corners of the frustum
//     static const glm::vec4 ndcCorners[8] = {
//         // near plane
//         {-1, -1, -1, 1},
//         {1, -1, -1, 1},
//         {1, 1, -1, 1},
//         {-1, 1, -1, 1},
//         // far plane
//         {-1, -1, 1, 1},
//         {1, -1, 1, 1},
//         {1, 1, 1, 1},
//         {-1, 1, 1, 1},
//     };
//
//     glm::mat4 invViewProj = glm::inverse(camera.proj * camera.view);
//
//     glm::vec3 corners[8];
//     for (int i = 0; i < 8; i++) {
//         glm::vec4 world = invViewProj * ndcCorners[i];
//         corners[i] = glm::vec3(world) / world.w; // perspective divide
//     }
//
//     // near rect, far rect, connections
//     std::vector<glm::vec3> lines = {
//         // near
//         corners[0],
//         corners[1],
//         corners[1],
//         corners[2],
//         corners[2],
//         corners[3],
//         corners[3],
//         corners[0],
//         // far
//         corners[4],
//         corners[5],
//         corners[5],
//         corners[6],
//         corners[6],
//         corners[7],
//         corners[7],
//         corners[4],
//         // connections
//         corners[0],
//         corners[4],
//         corners[1],
//         corners[5],
//         corners[2],
//         corners[6],
//         corners[3],
//         corners[7],
//     };
//
//     m_Renderer->drawLine(lines);
// }
//
// } // namespace zk
