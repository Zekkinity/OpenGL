#include "Renderer.hpp"

#include "ZkEngine/ModelObject.hpp"
#include "ZkEngine/RenderObject.hpp"
#include "imgui.h"

#include <iostream>

void APIENTRY openglDebugCallback(GLenum source,
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

void Renderer::init() {
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
}

void Renderer::update() {
    if (m_WireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Renderer::beginFrame() const {
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);

    // glClear(GL_COLOR_BUFFER_BIT);

    // 3D
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::submit(const RenderObject& obj, const Camera3D& camera) {
    if (!obj.mesh || !obj.material)
        return;

    m_DrawQueue.emplace_back(
        DrawCommand{
            obj.mesh,
            obj.material,
            obj.transform.getMatrix(),
            camera.getViewProjection(),
            obj.color,
            obj.colorOverride,
        }
    );
}

void Renderer::endFrame() {
    for (const auto& cmd : m_DrawQueue) {
        cmd.material->bind(cmd.viewProj, cmd.model);

        if (cmd.colorOverride) {
            cmd.material->getShader().setUniform4fv("u_Color", cmd.color);
        }

        cmd.mesh->bind();

        if (cmd.mesh->isTriangle()) {
            glDrawElements(GL_TRIANGLES, cmd.mesh->getIndexBuffer().getCount(), GL_UNSIGNED_INT, nullptr);
        } else {
            glDrawElements(GL_LINES, cmd.mesh->getIndexBuffer().getCount(), GL_UNSIGNED_INT, nullptr);
        }
    }

    m_DrawQueue.clear();
}

void Renderer::debug() {
    ImGui::ColorEdit4("Clear Color", &m_ClearColor[0]);
    ImGui::Checkbox("Wireframe", &m_WireFrame);
}
