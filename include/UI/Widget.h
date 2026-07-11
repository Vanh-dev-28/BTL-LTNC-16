#pragma once

namespace SpaceInvaders {

class Renderer;

/** Base type for reusable user-interface controls. */
class Widget {
public:
    virtual ~Widget() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render(Renderer& renderer) = 0;
};

} // namespace SpaceInvaders
