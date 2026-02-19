#pragma once

namespace zk {

class Scene;
class Shader;

class LightSystem {
  public:
    LightSystem(Scene& scene);
    ~LightSystem();

    void upload(Shader& shader);

    void debug();

  private:
    Scene& m_Scene;
};

} // namespace zk
