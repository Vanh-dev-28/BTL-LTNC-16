#pragma once

namespace SpaceInvaders {

class Renderer;

/** Defines the lifecycle and frame operations for an application scene. */
class Scene {
public:
    virtual ~Scene() = default;

    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(Renderer& renderer) = 0;
};

} // namespace SpaceInvaders
