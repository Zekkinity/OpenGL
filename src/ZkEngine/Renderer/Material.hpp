#pragma once

#include <memory>
#include "ZkEngine/Renderer/Shader.hpp"
#include "ZkEngine/Renderer/Texture.hpp"

namespace zk {

enum class BlendMode {
    OPAQUE,
    TRANSPARENT,
};

class Material {
  public:
    Material() = delete;
    Material(
        std::shared_ptr<Shader> shader,
        std::shared_ptr<Texture> texture = nullptr,
        std::shared_ptr<Texture> specularMap = nullptr);

    void bind() const;
    void unbind() const;

    // Textures
    void setDiffuse(std::shared_ptr<Texture> texture) { m_Texture = texture; }
    void setSpecular(std::shared_ptr<Texture> texture) { m_SpecularMap = texture; }
    void setShininess(float shininess) { m_Shininess = shininess; }
    void setTextures(bool enable) { m_UseTextures = enable; }

    // Color
    void setColor(const glm::vec4& color) {
        if (color.a < 1.0f)
            m_BlendMode = BlendMode::TRANSPARENT;
        else
            m_BlendMode = BlendMode::OPAQUE;
        m_Color = color;
    }

    void setBlendMode(BlendMode mode) { m_BlendMode = mode; }

    bool isTransparent() const { return m_BlendMode != BlendMode::OPAQUE; }

    Shader& getShader() const { return *m_Shader; }

  private:
    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<Texture> m_Texture;
    std::shared_ptr<Texture> m_SpecularMap;

    float m_Shininess = 64.0f;

    glm::vec4 m_Color = glm::vec4(1.0f);

    BlendMode m_BlendMode = BlendMode::OPAQUE;

    bool m_UseTextures = true;
};

} // namespace zk
