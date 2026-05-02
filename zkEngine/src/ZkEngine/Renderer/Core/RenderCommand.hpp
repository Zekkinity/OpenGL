#pragma once

namespace zk {

class Mesh;
class Material;

struct RenderCommand {
    Ref<Mesh> mesh;
    Ref<Material> material;
    glm::mat4 modelMatrix;
    float distanceToCamera;
};

} // namespace zk
