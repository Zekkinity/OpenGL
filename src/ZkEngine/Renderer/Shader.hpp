#pragma once

#include <memory>
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

    virtual void setUniform1i(const std::string& name, int value) = 0;
    virtual void setUniform1f(const std::string& name, float value) = 0;
    virtual void setUniform3f(const std::string& name, float v0, float v1, float v2) = 0;
    virtual void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) = 0;
    virtual void setUniform3fv(const std::string& name, const glm::vec3& value) = 0;
    virtual void setUniform4fv(const std::string& name, const glm::vec4& value) = 0;
    virtual void setUniformMat4f(const std::string& name, const glm::mat4& matrix) = 0;

    virtual void setLightUniform(
        const std::string& name, const PointLight& light, const glm::vec3& position) = 0;
    virtual void setLightUniform(const std::string& name, const DirectionalLight& light) = 0;
    virtual void setLightUniform(
        const std::string& name, const SpotLight& light, const glm::vec3& position) = 0;

    static std::shared_ptr<Shader> create(
        const std::string& vertexShader, const std::string& fragmentShader);
};

} // namespace zk
