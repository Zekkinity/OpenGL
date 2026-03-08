#pragma once

namespace zk {

class Registry;
class Shader;

class LightSystem {
  public:
    LightSystem(Registry& registry);
    ~LightSystem();

    void upload(Shader& shader);

    void debug();

  private:
    Registry& m_Registry;
};

} // namespace zk
