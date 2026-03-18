#include "Material.hpp"

#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

Material::Material(
    std::shared_ptr<Shader> shader,
    std::shared_ptr<Texture> texture,
    std::shared_ptr<Texture> specularMap)
    : m_Shader(shader), m_Texture(texture), m_SpecularMap(specularMap) {
    // This texture will help with non-textured objects
    glGenTextures(1, &white);
    glBindTexture(GL_TEXTURE_2D, white);

    unsigned int pixel = 0xffffffff;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
}

void Material::bind() const {
    m_Shader->bind();

    m_Shader->setFloat("u_Material.shininess", m_Shininess);
    m_Shader->setVec4("u_Color", m_Color);

    if (m_Texture) {
        m_Texture->bind(0);
        m_Shader->setInt("u_Material.diffuse", 0);
    }
    else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, white);
    }

    if (m_SpecularMap) {
        m_SpecularMap->bind(1);
        m_Shader->setInt("u_Material.specular", 1);
    }
    else {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, white);
    }

    m_Shader->setInt("u_UseTextures", m_UseTextures);
}

void Material::unbind() const {
    if (m_Texture)
        m_Texture->unbind(0);
    if (m_SpecularMap)
        m_SpecularMap->unbind(1);

    m_Shader->unbind();
}

} // namespace zk
