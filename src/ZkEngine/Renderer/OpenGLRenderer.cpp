#include "OpenGLRenderer.hpp"

#include "ZkEngine/Core/Base.hpp"
#include "ZkEngine/Renderer/CameraUBO.hpp"
#include "ZkEngine/Renderer/Framebuffer.hpp"
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

    std::string sourceStr;
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        sourceStr = "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        sourceStr = "Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        sourceStr = "Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        sourceStr = "Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        sourceStr = "Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        sourceStr = "Other";
        break;
    }

    std::string typeStr;
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        typeStr = "Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typeStr = "Deprecated Behavior";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typeStr = "Undefined Behavior";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        typeStr = "Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        typeStr = "Performance";
        break;
    case GL_DEBUG_TYPE_OTHER:
        typeStr = "Other";
        break;
    }

    std::string severityStr;
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        severityStr = "High";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        severityStr = "Medium";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        severityStr = "Low";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severityStr = "Notification";
        break;
    }

    ZK_CORE_ERROR("[{} {}] {}", sourceStr, typeStr, message);
}

void OpenGLRenderer::init() {
    // Blending
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

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Anti Aliasing
    glEnable(GL_MULTISAMPLE);

    ZK_CORE_INFO("OpenGL version: {}", (const char*)glGetString(GL_VERSION));

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    ZK_CORE_INFO("Maximum nr of vertex attributes supported: {}", nrAttributes);

    // Line stuff
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

    switch (m_RenderMode) {
    case RenderMode::Forward:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;

    case RenderMode::Deferred:
        break;

    case RenderMode::Wireframe:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;

    default:
        break;
    }
}

void OpenGLRenderer::beginFrame() const {
    glEnable(GL_DEPTH_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);

    // glClear(GL_COLOR_BUFFER_BIT);

    // 3D
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glStencilMask(0x00);
}

void OpenGLRenderer::submit(const RenderCommand& cmd) {
    if (cmd.material->isTransparent())
        m_TransparentQueue.emplace_back(cmd);
    else
        m_OpaqueQueue.emplace_back(cmd);
}

/*
    TODO:
    - Drawing order matters for blending and stencil
    - Blending
        - Draw all opaque objects first
        - Sort all transparent objects
        - Draw all transparent objects in sorted order
*/
void OpenGLRenderer::endFrame() {
    for (const auto& cmd : m_OpaqueQueue) {
        draw(cmd);
    }
    m_OpaqueQueue.clear();

    // TODO:
    // Order Independent Transparency

    // (farthest first)
    sortTransparentQueue();
    for (const auto& cmd : m_TransparentQueue) {
        draw(cmd);
    }
    m_TransparentQueue.clear();

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

    ImGui::Combo(
        "Render Mode", reinterpret_cast<int*>(&m_RenderMode), "Forward\0Deferred\0Wireframe\0\0");
    ImGui::Checkbox("Enable Textures", &m_EnableTextures);
}

void OpenGLRenderer::draw(const RenderCommand& cmd) {
    if (!cmd.mesh) {
        std::cout << "No Mesh for render command\n";
        return;
    }

    if (!cmd.material) {
        std::cout << "No Material for render command\n";
        return;
    }

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    // DrawTwoContainers();

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    // DrawTwoScaledUpContainers();

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);

    cmd.material->setTextures(m_EnableTextures);

    // TODO: Dont need to bind every time
    // could sort by shader, material, mesh
    cmd.material->bind();
    cmd.mesh->bind();

    Shader& shader = cmd.material->getShader();
    shader.setUniformMat4f("u_Model", cmd.modelMatrix);

    if (cmd.mesh->isTriangle()) {
        glDrawElements(
            GL_TRIANGLES, cmd.mesh->getIndexBuffer().getCount(), GL_UNSIGNED_INT, nullptr);
    }
    else {
        glDrawElements(GL_LINES, cmd.mesh->getIndexBuffer().getCount(), GL_UNSIGNED_INT, nullptr);
    }

    // cmd.mesh->unbind();
    // cmd.material->unbind();

    m_DrawCallCount++;
}

// Sort transparent objects by farthest first
void OpenGLRenderer::sortTransparentQueue() {
    std::sort(
        m_TransparentQueue.begin(),
        m_TransparentQueue.end(),
        [](const RenderCommand& a, const RenderCommand& b) {
            return a.distanceToCamera > b.distanceToCamera;
        });
}

} // namespace zk
