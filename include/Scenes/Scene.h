#pragma once

namespace SpaceInvaders {


class Renderer;
class SceneManager;

/** Defines the lifecycle and frame operations for an application scene. */
class Scene {
protected:
    SceneManager* sceneManager_ = nullptr;
public:
    virtual ~Scene() = default;

    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(Renderer& renderer) = 0;
    void setSceneManager(SceneManager* manager)
{
    sceneManager_ = manager;
}
};

} // namespace SpaceInvaders
