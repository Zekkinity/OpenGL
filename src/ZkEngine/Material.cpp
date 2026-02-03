#include "Material.hpp"

namespace zk {

Material::Material(
    std::shared_ptr<Shader> shader,
    std::shared_ptr<Texture> texture,
    std::shared_ptr<Texture> specularMap)
    : m_Shader(shader), m_Texture(texture), m_SpecularMap(specularMap) {}

void Material::bind(const glm::mat4& viewProj, const glm::mat4& model) const {
    glm::mat4 mvp = viewProj * model;

    m_Shader->bind();

    m_Shader->setUniformMat4f("u_MVP", mvp);
    m_Shader->setUniformMat4f("u_Model", model);

    m_Shader->setUniform4fv("u_Color", m_Color);

    m_Shader->setUniform1f("u_ToonSteps", m_ToonSteps);

    if (m_Texture) {
        m_Texture->bind(0);
        m_Shader->setUniform1i("u_Texture", 0);
    }

    if (m_SpecularMap) {
        m_SpecularMap->bind(1);
        m_Shader->setUniform1i("u_SpecularMap", 1);
    }
}

void Material::unbind() const {
    m_Shader->unbind();
}

} // namespace zk
