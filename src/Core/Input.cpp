#include "Core/Input.h"

namespace SpaceInvaders
{

    void Input::update()
    {
        keyboardState_ = SDL_GetKeyboardState(nullptr);
        mouseButtons_ = SDL_GetGlobalMouseState(nullptr, nullptr);
    }

    bool Input::isKeyPressed(SDL_Scancode key) const
    {
        return keyboardState_ != nullptr && keyboardState_[key];
    }

    bool Input::isMousePressed(Uint32 button) const
    {
        return (mouseButtons_ & SDL_BUTTON_MASK(button)) != 0U;
    }

} // namespace SpaceInvaders
