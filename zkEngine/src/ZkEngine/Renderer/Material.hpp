#pragma once

#include "ZkEngine/Core/Base.hpp"
#include "glm/ext/vector_float4.hpp"

namespace zk {

class Shader;
class Texture;
class AssetManager;

enum class BlendMode {
    OPAQUE,
    TRANSPARENT,
};

struct MaterialSpecs {
    Ref<Shader> shader = nullptr;
    Ref<Texture> diffuse = nullptr;
    Ref<Texture> specular = nullptr;
    Ref<Texture> normal = nullptr;
    Ref<Texture> height = nullptr;

    Ref<Texture> metallic = nullptr;
    Ref<Texture> roughness = nullptr;

    Ref<Texture> metallicRoughness = nullptr;
    Ref<Texture> ao = nullptr;
};

class Material {
  public:
    Material() = delete;
    Material(const MaterialSpecs& specs, AssetManager* assetManager);
    ~Material() = default;

    void bindTextures() const;
    void uploadUniforms(Shader& shader) const;

    void unbind() const;

    // Override default GBuffer shader
    void setShader(Ref<Shader> shader) { m_Shader = shader; }

    // Textures
    void setDiffuse(Ref<Texture> texture) { m_Specs.diffuse = texture; }
    void setSpecular(Ref<Texture> texture) { m_Specs.specular = texture; }
    void setShininess(float shininess) { m_Shininess = shininess; }
    void setNormal(Ref<Texture> texture) { m_Specs.normal = texture; }

    // gltf metallic roughness
    void setMetallicRoughness(Ref<Texture> metallicRoughness) {
        m_Specs.metallicRoughness = metallicRoughness;
    }

    void setAmbientOcclusion(Ref<Texture> ao) { m_Specs.ao = ao; }

    // For individual textures
    void setMetallic(Ref<Texture> texture) { m_Specs.metallic = texture; }
    void setRoughness(Ref<Texture> texture) { m_Specs.roughness = texture; }

    void setTextures(bool enable) { m_UseTextures = enable; }
    void setNormalMapping(bool enable) { m_NormalMapping = enable; }

    Ref<Texture> getDiffuse() const;

    // Color
    void setColor(const glm::vec4& color) {
        if (color.a < 1.0f)
            m_BlendMode = BlendMode::TRANSPARENT;
        else
            m_BlendMode = BlendMode::OPAQUE;
        m_Color = color;
    }
    const glm::vec4& getColor() const { return m_Color; }

    void setBlendMode(BlendMode mode) { m_BlendMode = mode; }

    bool isTransparent() const { return m_BlendMode != BlendMode::OPAQUE; }

    Shader& getShader() const { return *m_Shader; }

  private:
    MaterialSpecs m_Specs;
    Ref<Shader> m_Shader;

    AssetManager* m_AssetManager = nullptr;

    float m_Shininess = 64.0f;

    glm::vec4 m_Color = glm::vec4(1.0f);

    BlendMode m_BlendMode = BlendMode::OPAQUE;

    bool m_UseTextures = true;
    bool m_NormalMapping = true;
};

} // namespace zk
