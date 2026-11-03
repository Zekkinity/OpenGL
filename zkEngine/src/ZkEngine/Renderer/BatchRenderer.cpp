#include "zkpch.hpp"

#include "BatchRenderer.hpp"

#include "ZkEngine/Renderer/Core/RenderQueue.hpp"
#include "ZkEngine/Renderer/Core/RenderCommand.hpp"

#include "ZkEngine/Renderer/Mesh.hpp"
#include "ZkEngine/Renderer/Material.hpp"

#include "ZkEngine/Renderer/API/OpenGL/InstanceBuffer.hpp"
#include "ZkEngine/Renderer/RendererBackend.hpp"

// TODO: Remove glad from here
#include <glad/glad.h>

#include "imgui.h"

namespace zk {

BatchRenderer::BatchRenderer(RenderDevice& device) : m_Device(&device) {
    m_InstanceBuffer = createScope<InstanceBuffer>(sizeof(InstanceData) * 1000);
}

void BatchRenderer::build(const RenderQueue& m_RenderQueue) {
    m_AllInstances.clear();
    m_Batches.clear();

    Batch currentBatch{};  // empty batch
    BatchKey currentKey{}; // empty key

    bool first = true;

    m_AllInstances.reserve(m_RenderQueue.getOpaque().size());
    m_Batches.reserve(m_RenderQueue.getOpaque().size());

    // Group into batches
    for (const auto& cmd : m_RenderQueue.getOpaque()) {
        if (!cmd.mesh->isInstanced()) {
            setupInstancing(cmd.mesh.get());
            cmd.mesh->setInstanced(true);
        }

        BatchKey key{cmd.mesh, cmd.material};

        // New batch
        if (first || key != currentKey) {
            // Save previous batch
            if (!first) {
                m_Batches.push_back(currentBatch);
            }

            // Start new batch
            currentBatch = {};
            currentBatch.key = key;
            currentKey = key;

            currentBatch.instanceCount = 0;
            currentBatch.instanceOffset = m_AllInstances.size();

            first = false;
        }

        InstanceData instance;
        instance.model = cmd.modelMatrix;
        instance.color = cmd.material->getColor();

        m_AllInstances.push_back(instance);
        currentBatch.instanceCount++;
    }

    // Save last batch
    if (!first) {
        m_Batches.push_back(currentBatch);
    }
}

void BatchRenderer::updateInstances() {
    // NOTE:
    // Could create the instance buffer here with the correct size and prevent resizing

    // Update instance buffer
    m_InstanceBuffer->update(m_AllInstances.data(), m_AllInstances.size() * sizeof(InstanceData));
    m_InstanceBuffer->bind();
}

void BatchRenderer::setupInstancing(Mesh* mesh) {
    mesh->bind();

    m_InstanceBuffer->bind();

    // mat4 (locations 5 - 8)
    for (int i = 0; i < 4; i++) {
        int index = 5 + i;
        glEnableVertexAttribArray(index);

        glVertexAttribPointer(
            index,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(InstanceData),
            (void*)(offsetof(InstanceData, model) + i * sizeof(glm::vec4)));

        glVertexAttribDivisor(index, 1);
    }

    // vec4 (location 9)
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(
        9, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, color));
    glVertexAttribDivisor(9, 1);

    mesh->unbind();
}

void BatchRenderer::debug() {
    ImGui::Text("Batching");

    ImGui::Text("Batches: %zu", m_Batches.size());
    ImGui::Text("Instances: %zu", m_AllInstances.size());
}

} // namespace zk
