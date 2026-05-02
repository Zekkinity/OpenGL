#pragma once

namespace zk {

class Mesh;
class Material;
class Shader;

// Batching
struct BatchKey {
    Ref<Mesh> mesh;
    Ref<Material> material;

    bool operator==(const BatchKey& other) const {
        return mesh == other.mesh && material == other.material;
    }
};

struct Batch {
    BatchKey key;

    size_t instanceOffset = 0;
    size_t instanceCount = 0;
};

struct InstanceData {
    glm::mat4 model;
    glm::vec4 color;
};

struct DrawContext {
    Shader* shader = nullptr;
    std::function<void(Shader&)> bindShader;
    std::function<void(Material*, Shader&)> bindMaterial;
};

class RenderQueue;
class RenderDevice;
class DrawContext;
class InstanceBuffer;

class BatchRenderer {
  public:
    BatchRenderer(RenderDevice& device);
    virtual ~BatchRenderer() = default;

    void build(const RenderQueue& m_RenderQueue);
    void updateInstances();
    void draw(const DrawContext& ctx);

    void debug();

  private:
    void setupInstancing(Mesh* mesh);

  private:
    std::vector<Batch> m_Batches;
    std::vector<InstanceData> m_AllInstances;
    Scope<InstanceBuffer> m_InstanceBuffer;
    Shader* m_CurrentShader = nullptr;
    Material* m_CurrentMaterial = nullptr;

    RenderDevice* m_Device = nullptr;
};

} // namespace zk
