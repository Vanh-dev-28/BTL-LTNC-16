#pragma once

#include "Utils/Vector2.h"

namespace SpaceInvaders {

class Renderer;

/** Base type for renderable world objects managed by future gameplay code. */
class Entity {
public:
    virtual ~Entity() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render(Renderer& renderer) = 0;
    virtual void destroy() = 0;
    [[nodiscard]] virtual Vector2 getBounds() const;

protected:
    Vector2 position_ {};
    Vector2 velocity_ {};
    Vector2 size_ {};
};

} // namespace SpaceInvaders
