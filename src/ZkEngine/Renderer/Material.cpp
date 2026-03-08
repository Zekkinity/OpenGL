#include "Material.hpp"

namespace zk {

Material::Material(
    std::shared_ptr<Shader> shader,
    std::shared_ptr<Texture> texture,
    std::shared_ptr<Texture> specularMap)
    : m_Shader(shader), m_Texture(texture), m_SpecularMap(specularMap) {}

void Material::bind() const {
    if (isTransparent()) {
        // glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else {
        // glDisable(GL_BLEND);
    }

    m_Shader->bind();

    m_Shader->setUniform1f("u_Material.shininess", m_Shininess);
    m_Shader->setUniform4fv("u_Color", m_Color);

    if (m_Texture) {
        m_Texture->bind(0);
        m_Shader->setUniform1i("u_Material.diffuse", 0);
    }

    if (m_SpecularMap) {
        m_SpecularMap->bind(1);
        m_Shader->setUniform1i("u_Material.specular", 1);
    }

    m_Shader->setUniform1i("u_UseTextures", m_UseTextures);
}

void Material::unbind() const {
    m_Texture->unbind();
    m_SpecularMap->unbind();
    m_Shader->unbind();
}

} // namespace zk
