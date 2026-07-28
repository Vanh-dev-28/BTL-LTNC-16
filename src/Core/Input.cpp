#include "Core/Input.h"

namespace SpaceInvaders
{

    void Input::update()
    {
        for (int i = 0; i < SDL_SCANCODE_COUNT; i++)
        {
            previousKeys_[i] = currentKeys_[i];
        }

        keyboardState_ = SDL_GetKeyboardState(nullptr);

        for (int i = 0; i < SDL_SCANCODE_COUNT; i++)
        {
            currentKeys_[i] = keyboardState_[i];
        }

        mouseButtons_ = SDL_GetGlobalMouseState(nullptr, nullptr);
    }
    bool Input::isKeyDown(SDL_Scancode key) const
    {
        return currentKeys_[key];
    }

    bool Input::isKeyPressed(SDL_Scancode key) const
    {
        return currentKeys_[key] && !previousKeys_[key];
    }

    bool Input::isKeyReleased(SDL_Scancode key) const
    {
        return !currentKeys_[key] && previousKeys_[key];
    }

    bool Input::isMousePressed(Uint32 button) const
    {
        return (mouseButtons_ & SDL_BUTTON_MASK(button)) != 0U;
    }

} // namespace SpaceInvaders
