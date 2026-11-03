#include "zkpch.hpp"
#include "Material.hpp"

#include "ZkEngine/Asset/AssetManager.hpp"
#include "ZkEngine/Renderer/Shader.hpp"
#include "ZkEngine/Renderer/Texture.hpp"
#include "ZkEngine/Core/AppAcess.hpp"

#include <glad/glad.h>

namespace zk {

Material::Material(const MaterialSpecs& specs, AssetManager* assetManager)
    : m_Specs(specs), m_AssetManager(assetManager) {
    m_Shader = Asset::loadShader("shaders/GBuffer.vert", "shaders/GBuffer.frag");
}

void Material::bindTextures() const {
    // Diffuse
    if (!m_UseTextures || !m_Specs.diffuse) {
        m_AssetManager->getWhiteTexture()->bind(0);
    }
    else {
        m_Specs.diffuse->bind(0);
    }

    // Specular
    m_Specs.specular ? m_Specs.specular->bind(1) : m_AssetManager->getBlackTexture()->bind(1);

    // Normal
    m_Specs.normal ? m_Specs.normal->bind(2) : m_AssetManager->getDefaultNormal()->bind(2);

    // Height
    m_Specs.height ? m_Specs.height->bind(3) : m_AssetManager->getBlackTexture()->bind(3);

    // Ambient Occlusion
    m_Specs.ao ? m_Specs.ao->bind(4) : m_AssetManager->getWhiteTexture()->bind(4);

    // Roughness
    m_Specs.roughness ? m_Specs.roughness->bind(5) : m_AssetManager->getWhiteTexture()->bind(5);

    // Metallic
    m_Specs.metallic ? m_Specs.metallic->bind(6) : m_AssetManager->getBlackTexture()->bind(6);

    // if not individual textures
    if ((!m_Specs.metallic && !m_Specs.roughness) && m_Specs.metallicRoughness) {
        m_Specs.metallicRoughness->bind(7);
    }
    else {
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Material::uploadUniforms(Shader& shader) const {
    shader.setInt("u_Material.diffuse", 0);
    shader.setInt("u_Material.specular", 1);
    shader.setInt("u_Material.normal", 2);
    shader.setInt("u_Material.depth", 3);

    shader.setInt("u_Material.ambientOcclusion", 4);
    shader.setInt("u_Material.roughness", 5);
    shader.setInt("u_Material.metallic", 6);

    shader.setInt("u_Material.metallicRoughness", 7);

    shader.setFloat("u_Material.shininess", m_Shininess);
    shader.setVec4("u_Color", m_Color);

    shader.setInt("u_NormalMapping", m_NormalMapping);
}

void Material::unbind() const {
    if (m_Specs.diffuse)
        m_Specs.diffuse->unbind(0);
    if (m_Specs.specular)
        m_Specs.specular->unbind(1);
    if (m_Specs.normal)
        m_Specs.normal->unbind(2);

    m_Specs.shader->unbind();
}

Ref<Texture> Material::getDiffuse() const {
    return m_Specs.diffuse;
}

} // namespace zk
