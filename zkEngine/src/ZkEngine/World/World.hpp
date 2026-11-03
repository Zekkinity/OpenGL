#pragma once

#include <entt/entt.hpp>

#include "ZkEngine/Asset/AssetManager.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLMesh.hpp"
#include "ZkEngine/World/Lights.hpp"

#include "ZkEngine/World/Camera3D.hpp"
#include "ZkEngine/Physics/PhysicsSystem.hpp"

#include "ZkEngine/World/Entity.hpp"

namespace zk {

struct Frustum {
    struct Plane {
        glm::vec3 normal;
        float distance;

        float getDistanceToPoint(const glm::vec3& point) const {
            return glm::dot(normal, point) + distance;
        }
    };

    Plane planes[6];
};

struct CameraData {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 position;
    Frustum frustum;
    float nearPlane;
    float farPlane;
};

struct RenderView;
class RendererBackend;
class PlayerController;
class Shader;

using EntityHandle = entt::entity;

class World {
  public:
    virtual ~World();

    World(const World&) = delete;
    World& operator=(const World&) = delete;

    World(World&&) = delete;
    World& operator=(World&&) = delete;

    void onInit(AssetManager* assetManager);
    void onUpdate(float dt);
    void onDebug();

    void onWindowResize(int width, int height);

    /*
        Override init() for setup, NOT a constructor.
        World construction is managed by the engine.
    */
    virtual void init() = 0;
    virtual void update(float dt) = 0;
    virtual void debug() {};

    LightData getLights() const;

    Entity createEntity();
    void destroyEntity(Entity entity);
    void clear();

    const entt::registry& getRegistry() const { return m_Registry; }

    CameraData getCameraData(Entity camera) const;

    PlayerController& getPlayerController() { return *m_PlayerController; }

    void swapCameras();

    std::vector<Entity> getCameras() const;

    Entity getActiveCamera() const;
    Entity getEditorCamera() const;

    void setActiveCamera(Entity newCamera);

    std::vector<RenderView> getRenderViews(RendererBackend& renderer) const;

  protected:
    // Initialization belongs to init() dont do it here
    World();

    AssetManager& GetAssets() { return *m_AssetManager; }

    Entity SpawnPointLight(const PointLight& light = {});
    Entity SpawnDirLight(const DirectionalLight& light = {});

  private:
    Entity createCamera(const CameraConfig& config = {});
    Frustum getFrustum(const glm::mat4& proj, const glm::mat4& view) const;

    void getEntity();

  private:
    entt::registry m_Registry;
    // std::unordered_map<UUID, entt::entity> m_EnityMap;

    Entity m_SelectedEntity;

    PhysicsSystem m_PhysicsSystem;

    AssetManager* m_AssetManager = nullptr;

    EntityHandle m_ActiveCamera = entt::null;
    EntityHandle m_EditorCamera = entt::null;

    Scope<PlayerController> m_PlayerController;

    friend class Entity;
    friend class WorldManager;
};

} // namespace zk
