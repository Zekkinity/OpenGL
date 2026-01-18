#pragma once

#include <memory>
#include "ZkEngine/Shader.hpp"
#include "ZkEngine/Texture.hpp"

class Material {
   public:
    Material(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture);

    void bind(const glm::mat4& viewProj, const glm::mat4& model) const;

    Shader& getShader() const { return *m_Shader; }

    void setColor(const glm::vec4& color) { m_Color = color; }
    void setToonSteps(float steps) { m_ToonSteps = steps; }

    void setLightDir(const glm::vec3& dir) { m_LightDir = dir; }

   private:
    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<Texture> m_Texture;

    glm::vec4 m_Color = {1, 1, 1, 1};
    float m_ToonSteps = 4.0f;

    glm::vec3 m_LightDir = {-1, -1, -1};
};
