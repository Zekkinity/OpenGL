#include "DebugPass.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"

namespace zk {

DebugPass::DebugPass(OpenGLRenderer* renderer) : m_Renderer(renderer) {
    m_DebugNormals = Asset::loadShader(
        "shaders/VisualizeNormals.vert",
        "shaders/VisualizeNormals.frag",
        "shaders/VisualizeNormals.geom");
}

void DebugPass::execute(const RenderContext& ctx) {
    if (!m_DebugNormalsEnabled)
        return;

    // glDepthMask(GL_FALSE);
    m_DebugNormals->bind();

    for (const auto& cmd : ctx.queue->getOpaque()) {
        cmd.mesh->bind();

        m_DebugNormals->setMat4("u_Model", cmd.modelMatrix);

        m_Renderer->drawElements(GL_TRIANGLES, cmd.mesh->getIndexBuffer().getCount());
    }

    for (const auto& cmd : ctx.queue->getTransparent()) {
        cmd.mesh->bind();

        m_DebugNormals->setMat4("u_Model", cmd.modelMatrix);

        m_Renderer->drawElements(GL_TRIANGLES, cmd.mesh->getIndexBuffer().getCount());
    }

    // glDepthMask(GL_TRUE);
}

void DebugPass::debug() {
    ImGui::Checkbox("Enable Normals", &m_DebugNormalsEnabled);
}

} // namespace zk
