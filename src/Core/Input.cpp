#include "Core/Input.h"
#include "Core/Renderer.h"

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
    previousMouseButtons_ = mouseButtons_;

    float windowMouseX;
    float windowMouseY;

    mouseButtons_ = SDL_GetMouseState(&windowMouseX, &windowMouseY);

    if (Renderer::s_scale > 0.0f)
    {
        mouseX_ =
            (windowMouseX - Renderer::s_offsetX)
            / Renderer::s_scale;

        mouseY_ =
            (windowMouseY - Renderer::s_offsetY)
            / Renderer::s_scale;
    }
    else
    {
        mouseX_ = windowMouseX;
        mouseY_ = windowMouseY;
    }
}

bool Input::isKeyDown(SDL_Scancode key) const
{
    return currentKeys_[key];
}

bool Input::isKeyPressed(SDL_Scancode key) const
{
    return currentKeys_[key] &&
           !previousKeys_[key];
}

bool Input::isKeyReleased(SDL_Scancode key) const
{
    return !currentKeys_[key] &&
           previousKeys_[key];
}

bool Input::isMouseDown(Uint32 button) const
{
    return (mouseButtons_ & SDL_BUTTON_MASK(button)) != 0U;
}

bool Input::isMousePressed(Uint32 button) const
{
    const Uint32 mask = SDL_BUTTON_MASK(button);

    return (mouseButtons_ & mask) != 0U &&
           (previousMouseButtons_ & mask) == 0U;
}

bool Input::isMouseReleaed(Uint32 button) const
{
    const Uint32 mask = SDL_BUTTON_MASK(button);

    return (mouseButtons_ & mask) == 0U &&
           (previousMouseButtons_ & mask) != 0U;
}

float Input::getMouseX() const
{
    return mouseX_;
}

float Input::getMouseY() const
{
    return mouseY_;
}

} // namespace SpaceInvaders