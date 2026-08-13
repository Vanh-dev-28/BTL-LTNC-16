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
void Input::handleEvent(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_TEXT_INPUT)
    {
        if (textInput_.length() < 16)
        {
            textInput_ += event.text.text;

            if (textInput_.length() > 16)
            {
                textInput_.resize(16);
            }
        }
    }
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        if (event.key.scancode == SDL_SCANCODE_BACKSPACE)
        {
            if (!textInput_.empty())
            {
                textInput_.pop_back();
            }
        }
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

bool Input::isMouseReleased(Uint32 button) const
{
    const Uint32 mask = SDL_BUTTON_MASK(button);

    return (mouseButtons_ & mask) == 0U &&
           (previousMouseButtons_ & mask) != 0U;
}
void Input::startTextInput()
{
    textInput_.clear();
    SDL_StartTextInput(SDL_GetKeyboardFocus());
}

void Input::stopTextInput()
{
    SDL_StopTextInput(SDL_GetKeyboardFocus());
}

const std::string& Input::getTextInput() const
{
    return textInput_;
}

void Input::clearTextInput()
{
    textInput_.clear();
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