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
};

class Material {
  public:
    Material() = delete;
    Material(const MaterialSpecs& specs, AssetManager* assetManager);
    ~Material() = default;

    void bindTextures() const;
    void uploadUniforms(Shader& shader) const;

    void unbind() const;

    void setShader(Ref<Shader> shader) { m_Specs.shader = shader; }

    // Textures
    void setDiffuse(Ref<Texture> texture) { m_Specs.diffuse = texture; }
    void setSpecular(Ref<Texture> texture) { m_Specs.specular = texture; }
    void setShininess(float shininess) { m_Shininess = shininess; }
    void setNormal(Ref<Texture> texture) { m_Specs.normal = texture; }

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

    Shader& getShader() const { return *m_Specs.shader; }

  private:
    MaterialSpecs m_Specs;

    AssetManager* m_AssetManager = nullptr;

    float m_Shininess = 64.0f;

    glm::vec4 m_Color = glm::vec4(1.0f);

    BlendMode m_BlendMode = BlendMode::OPAQUE;

    bool m_UseTextures = true;
    bool m_NormalMapping = true;
};

} // namespace zk
