#include "Entities/Entity.h"

namespace SpaceInvaders {

Vector2 Entity::getBounds() const
{
    return size_;
}

} // namespace SpaceInvaders
