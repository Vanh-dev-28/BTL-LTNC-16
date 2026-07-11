#include "Core/Input.h"

namespace SpaceInvaders {

void Input::update()
{
    keyboardState_ = SDL_GetKeyboardState(nullptr);
    mouseButtons_ = SDL_GetMouseState(nullptr, nullptr);
}

bool Input::isKeyPressed(SDL_Scancode key) const
{
    return keyboardState_ != nullptr && keyboardState_[key] != 0;
}

bool Input::isMousePressed(Uint32 button) const
{
    return (mouseButtons_ & SDL_BUTTON(button)) != 0U;
}

} // namespace SpaceInvaders
