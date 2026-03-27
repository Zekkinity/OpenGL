#pragma once

#include <string>
#include <glm/glm.hpp>

#include "ZkEngine/Scene/Lights.hpp"

namespace zk {

// NOTE:
// This could inherit from Asset and get a uint64_t handle to make one map for all assets
class Shader {
  protected:
    Shader() = default;
    virtual ~Shader() = default;

  public:
    Shader(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&) = delete;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void setInt(const std::string& name, int value) = 0;
    virtual void setFloat(const std::string& name, float value) = 0;
    virtual void setFloat3(const std::string& name, float v0, float v1, float v2) = 0;
    virtual void setFloat4(const std::string& name, float v0, float v1, float v2, float v3) = 0;
    virtual void setVec3(const std::string& name, const glm::vec3& value) = 0;
    virtual void setVec4(const std::string& name, const glm::vec4& value) = 0;
    virtual void setMat4(const std::string& name, const glm::mat4& matrix) = 0;

    virtual void setLight(
        const std::string& name, const PointLight& light, const glm::vec3& position) = 0;
    virtual void setLight(const std::string& name, const DirectionalLight& light) = 0;
    virtual void setLight(
        const std::string& name, const SpotLight& light, const glm::vec3& position) = 0;

    virtual void reload() = 0;
};

} // namespace zk
