#include "OpenGLRenderer.hpp"

#include "ZkEngine/CameraUBO.hpp"
#include "imgui.h"

#include <iostream>

namespace zk {

void APIENTRY openglDebugCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam) {
    // Filter out non-critical messages if you want
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    std::cerr << "[OpenGL Debug] "
              << "Source: " << source << ", Type: " << type << ", ID: " << id
              << ", Severity: " << severity << "\nMessage: " << message << "\n\n";
}

void OpenGLRenderer::init() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // 3D
    glEnable(GL_DEPTH_TEST);

    if (m_Debug) {
        // Debug
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglDebugCallback, nullptr);
    }

    if (m_WireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Anti Aliasing
    glEnable(GL_MULTISAMPLE);

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << '\n';

    glGenVertexArrays(1, &m_LineVAO);
    glGenBuffers(1, &m_LineVBO);

    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);

    m_CameraUBO = std::make_unique<UniformBuffer>(sizeof(CameraUBO), 0);
}

void OpenGLRenderer::update() {
    m_DrawCallCount = 0;

    if (m_WireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void OpenGLRenderer::beginFrame() const {
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);

    // glClear(GL_COLOR_BUFFER_BIT);

    // 3D
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::submit(const RenderObject& obj) {
    if (!obj.mesh || !obj.material)
        return;

    m_DrawQueue.emplace_back(
        DrawCommand{
            obj.mesh,
            obj.material,
            obj.transform.getMatrix(),
            obj.color,
            obj.colorOverride,
        });
}

void OpenGLRenderer::submit(const RenderObject& obj, const Transform3D& transform) {
    if (!obj.mesh || !obj.material)
        return;

    m_DrawQueue.emplace_back(
        DrawCommand{
            obj.mesh,
            obj.material,
            transform.getMatrix(),
            obj.color,
            obj.colorOverride,
        });
}


void OpenGLRenderer::endFrame() {
    for (const auto& cmd : m_DrawQueue) {
        if (m_DisableTextures) {
            cmd.material->setDisableTextures(true);
        } else {
            cmd.material->setDisableTextures(false);
        }
        cmd.material->bind(cmd.model);

        if (cmd.colorOverride) {
            cmd.material->getShader().setUniform4fv("u_Color", cmd.color);
        }

        cmd.mesh->bind();

        if (cmd.mesh->isTriangle()) {
            glDrawElements(
                GL_TRIANGLES, cmd.mesh->getIndexBuffer().getCount(), GL_UNSIGNED_INT, nullptr);
        } else {
            glDrawElements(
                GL_LINES, cmd.mesh->getIndexBuffer().getCount(), GL_UNSIGNED_INT, nullptr);
        }

        // cmd.mesh->unbind();
        // cmd.material->unbind();

        m_DrawCallCount++;
    }

    m_DrawQueue.clear();

    glBindVertexArray(m_LineVAO);

    for (const auto& cmd : m_LineQueue) {

        glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);

        glBufferData(
            GL_ARRAY_BUFFER,
            cmd.points.size() * sizeof(glm::vec3),
            cmd.points.data(),
            GL_DYNAMIC_DRAW);

        // glDrawArrays(GL_LINE_STRIP, 0, cmd.points.size());
        glDrawArrays(GL_LINES, 0, cmd.points.size());

        m_DrawCallCount++;
    }

    m_LineQueue.clear();
}

void OpenGLRenderer::drawLine(const std::vector<glm::vec3>& pts) {
    m_LineQueue.emplace_back(LineStripCmd{pts});
}

void OpenGLRenderer::setClearColor(const glm::vec4& color) {
    m_ClearColor[0] = color.r;
    m_ClearColor[1] = color.g;
    m_ClearColor[2] = color.b;
    m_ClearColor[3] = color.a;

    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRenderer::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    glViewport(x, y, width, height);
}

void OpenGLRenderer::updateCamera(const Camera3D& camera) {
    CameraUBO data;
    data.viewProj = camera.getViewProjection();
    data.viewPos = glm::vec4(camera.getPosition(), 1.0f);
    m_CameraUBO->setData(&data, sizeof(CameraUBO));
}

void OpenGLRenderer::debug() {
    ImGui::Text("Draw Calls: %d", m_DrawCallCount);
    ImGui::ColorEdit4("Clear Color", &m_ClearColor[0]);
    ImGui::Checkbox("Wireframe", &m_WireFrame);
    ImGui::Checkbox("Disable Textures", &m_DisableTextures);
}

} // namespace zk
