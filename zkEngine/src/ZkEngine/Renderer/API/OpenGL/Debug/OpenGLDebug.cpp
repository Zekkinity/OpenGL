#include "zkpch.hpp"
#include "ZkEngine/Renderer/API/OpenGL/Debug/OpenGLDebug.hpp"

#include "ZkEngine/Core/Base.hpp"

#include <unordered_set>

namespace zk {

static std::unordered_set<uint32_t> s_LoggedErrors;

void OpenGLDebug::init(bool enabled) {
    if (!enabled) {
        return;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    ZK_CORE_INFO("OpenGL version: {}", (const char*)glGetString(GL_VERSION));
}

void APIENTRY OpenGLDebug::callback(
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

    if (s_LoggedErrors.contains(id)) {
        return;
    }

    s_LoggedErrors.insert(id);

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

    ZK_CORE_ERROR("[{} {} {}] {}", sourceStr, severityStr, typeStr, message);
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        ZK_DEBUGBREAK();
    }
}

} // namespace zk
