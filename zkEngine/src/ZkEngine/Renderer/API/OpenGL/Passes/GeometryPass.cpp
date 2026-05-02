// #include "zkpch.hpp"
// #include "GeometryPass.hpp"
//
// #include "ZkEngine/Utils/Profiler.hpp"
// #include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"
//
// #include "ZkEngine/Renderer/Core/RenderContext.hpp"
// #include "ZkEngine/Renderer/Shader.hpp"
// #include "ZkEngine/Renderer/Material.hpp"
// #include "ZkEngine/Utils/Time.hpp"
//
// #include "imgui.h"
//
// namespace zk {
//
// GeometryPass::GeometryPass(OpenGLRenderer* renderer) : m_Renderer(renderer) {}
//
// /*
//     TODO:
//     - Drawing order matters for blending and stencil
//     - Blending
//         - Draw all opaque objects first
//         - Sort all transparent objects
//         - Draw all transparent objects in sorted order
//     - Sort by Shader, material, mesh
//     - Group into batches
//     - add instance buffer
//     - indirect buffer
//     - use glMultiDrawElementsIndirect
// */
// void GeometryPass::execute(const RenderContext& ctx, const RenderView& view) {
//     // ZK_CORE_DEBUG("Rendering {0} opaque commands", ctx.queue->getOpaque().size());
//
//     if (m_Wireframe) {
//         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//     }
//
//     // glDisable(GL_BLEND);
//     // glDepthMask(GL_TRUE);
//
//     // instanceRendering(*ctx.queue, *ctx.lights);
//
//     // for (const auto& cmd : ctx.queue->getOpaque()) {
//     //     Shader& shader = cmd.material->getShader();
//     //
//     //     if (&shader != m_CurrentShader) {
//     //         shader.bind();
//     //         // TODO: Implement a UBO for lights insteaad of this
//     //         uploadLightsToShader(shader, *ctx.lights);
//     //
//     //         cmd.material->setTextures(m_EnableTextures);
//     //         cmd.material->setNormalMapping(m_EnableMapping);
//     //         cmd.material->setShininess(m_Shininess);
//     //
//     //         shader.setInt("u_Blinn", m_EnableBlinn);
//     //         shader.setFloat("time", Time::getTime());
//     //
//     //         m_CurrentShader = &shader;
//     //     }
//     //
//     //     draw(cmd);
//     //     cmd.material->unbind();
//     // }
//
//     // TODO:
//     // Order Independent Transparency
//
//     // glEnable(GL_BLEND);
//     // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//     // glDepthMask(GL_FALSE);
//
//     // ctx.queue->sortTransparent();
//     // for (const auto& cmd : ctx.queue->getTransparent()) {
//     //     draw(cmd);
//     // }
//
//     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
// }
//
// void GeometryPass::draw(const RenderCommand& cmd) {
//     TracyGpuZone("Draw Mesh");
//
//     if (!cmd.mesh) {
//         ZK_CORE_ERROR("No Mesh for render command");
//         return;
//     }
//
//     if (cmd.mesh->getIndexBuffer().getCount() == 0) {
//         ZK_CORE_ERROR("Mesh has zero indices");
//         return;
//     }
//
//     if (!cmd.material) {
//         ZK_CORE_ERROR("No Material for render command");
//         return;
//     }
//
//     cmd.material->bindTextures();
//     cmd.material->uploadUniforms(cmd.material->getShader());
//     cmd.material->getShader().setMat4("u_Model", cmd.modelMatrix);
//
//     if (cmd.mesh->isTriangle()) {
//         m_Renderer->drawMesh(cmd.mesh);
//     }
//     else {
//         m_Renderer->drawElements(GL_LINES, cmd.mesh->getIndexBuffer().getCount());
//     }
// }
//
// void GeometryPass::uploadLightsToShader(Shader& shader, const LightData& data) {
//     ZK_PROFILE_SCOPE("Upload Lights");
//
//     // 0 = diffuse
//     // 1 = specular
//     // 2 = free
//     // 3 = shadow dir
//     // 4-7 = shadow cube
//
//     shader.setInt("u_PointLightCount", data.pointLights.size());
//     shader.setInt("u_SpotLightCount", data.spotLights.size());
//     shader.setInt("u_DirLightCount", data.dirLights.size());
//
//     // Bind a dummy cubemap to all 4 slots first
//     for (int i = 0; i < 4; i++) {
//         glActiveTexture(GL_TEXTURE0 + 4 + i);
//         glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // unbind / default
//         shader.setInt("u_ShadowCubeMap" + std::to_string(i), 4 + i);
//         shader.setFloat("u_FarPlane" + std::to_string(i), 1.0f);
//     }
//
//     for (size_t i = 0; i < data.pointLights.size(); i++) {
//         auto& pointLight = data.pointLights[i];
//
//         shader.setLight(
//             "u_PointLights[" + std::to_string(i) + "]", pointLight.light, pointLight.position);
//
//         // Bind shadow map
//         int unit = 4 + i;
//         glActiveTexture(GL_TEXTURE0 + unit);
//         glBindTexture(GL_TEXTURE_CUBE_MAP, pointLight.depthMap);
//
//         // shader.setInt("u_ShadowCubeMap", unit);
//         // shader.setFloat("u_FarPlane", pointLight.farPlane);
//         shader.setInt("u_ShadowCubeMap" + std::to_string(i), unit);
//         shader.setFloat("u_FarPlane" + std::to_string(i), pointLight.farPlane);
//     }
//
//     for (size_t i = 0; i < data.spotLights.size(); i++) {
//         shader.setLight(
//             // "u_SpotLights[" + std::to_string(i) + "]",
//             "u_SpotLight",
//             data.spotLights[i].light,
//             data.spotLights[i].position);
//     }
//
//     // Directional lights (Shadow)
//     for (size_t i = 0; i < data.dirLights.size(); i++) {
//         auto& dirLight = data.dirLights[i];
//
//         int unit = 3;
//         glActiveTexture(GL_TEXTURE0 + unit);
//         glBindTexture(GL_TEXTURE_2D, dirLight.depthMap);
//
//         shader.setInt("u_DirLight.shadowMap", unit);
//         shader.setInt("u_DirShadowMap", unit);
//         shader.setMat4("u_LightSpaceMatrix", dirLight.lightSpaceMatrix);
//     }
// }
//
// void GeometryPass::debug() {
//     ImGui::Checkbox("Enable Wireframe", &m_Wireframe);
//     ImGui::Checkbox("Enable Textures", &m_EnableTextures);
//     ImGui::Checkbox("Enable Normal Mapping", &m_EnableMapping);
//     ImGui::SliderFloat("Shininess", &m_Shininess, 0.0f, 360.0f);
//     ImGui::Checkbox("Enable Blinn", &m_EnableBlinn);
// }
//
// // void GeometryPass::instanceRendering(const RenderQueue& queue, const LightData& data) {
// //     m_CurrentShader = nullptr;
// //     m_CurrentMaterial = nullptr;
// //
// //     m_AllInstances.clear();
// //     m_Batches.clear();
// //
// //     Batch currentBatch{};  // empty batch
// //     BatchKey currentKey{}; // empty key
// //
// //     bool first = true;
// //
// //     m_AllInstances.reserve(queue.getOpaque().size());
// //
// //     // Group into batches
// //     for (const auto& cmd : queue.getOpaque()) {
// //         if (!cmd.mesh->isInstanced()) {
// //             setupInstancing(cmd.mesh.get());
// //             cmd.mesh->setInstanced(true);
// //         }
// //
// //         BatchKey key{cmd.mesh, cmd.material};
// //
// //         // New batch
// //         if (first || key != currentKey) {
// //             // Save previous batch
// //             if (!first) {
// //                 m_Batches.push_back(currentBatch);
// //             }
// //
// //             // Start new batch
// //             currentBatch = {};
// //             currentBatch.key = key;
// //             currentKey = key;
// //
// //             currentBatch.instanceCount = 0;
// //             currentBatch.instanceOffset = m_AllInstances.size();
// //
// //             first = false;
// //         }
// //
// //         InstanceData instance;
// //         instance.model = cmd.modelMatrix;
// //         instance.color = cmd.material->getColor();
// //
// //         m_AllInstances.push_back(instance);
// //         currentBatch.instanceCount++;
// //     }
// //
// //     // Save last batch
// //     if (!first) {
// //         m_Batches.push_back(currentBatch);
// //     }
// //
// //     // Update instance buffer
// //     m_InstanceBuffer->update(m_AllInstances.data(), m_AllInstances.size() * sizeof(InstanceData));
// //     m_InstanceBuffer->bind();
// //
// //     // Draw Batches
// //     int i = 0;
// //     for (const auto& batch : m_Batches) {
// //         // ZK_CORE_DEBUG(
// //         //     "Batch {}/{}, Instances: {}, Offset: {}",
// //         //     ++i,
// //         //     m_Batches.size(),
// //         //     batch.instanceCount,
// //         //     batch.instanceOffset);
// //
// //         Shader& shader = batch.key.material->getShader();
// //         Material* material = batch.key.material.get();
// //
// //         // Update shader
// //         if (&shader != m_CurrentShader) {
// //             shader.bind();
// //
// //             shader.setInt("u_Blinn", m_EnableBlinn);
// //             shader.setFloat("time", Time::getTime());
// //
// //             m_CurrentShader = &shader;
// //         }
// //
// //         // Update material
// //         if (material != m_CurrentMaterial) {
// //             material->bindTextures();
// //             material->uploadUniforms(shader);
// //
// //             material->setTextures(m_EnableTextures);
// //             material->setNormalMapping(m_EnableMapping);
// //             material->setShininess(m_Shininess);
// //
// //             m_CurrentMaterial = material;
// //         }
// //
// //         uploadLightsToShader(shader, data);
// //
// //         batch.key.mesh->bind();
// //         m_Renderer->drawInstanced(batch.key.mesh->getIndexBuffer().getCount(), batch);
// //     }
// // }
//
// } // namespace zk
