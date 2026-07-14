#pragma once

#include <SDL3/SDL.h>

namespace SpaceInvaders
{

    /** Tracks keyboard and mouse state for the current input frame. */
    class Input
    {
    public:
        void update();
        [[nodiscard]] bool isKeyPressed(SDL_Scancode key) const;
        [[nodiscard]] bool isMousePressed(Uint32 button) const;

    private:
        const bool *keyboardState_{nullptr};
        Uint32 mouseButtons_{0U};
    };

} // namespace SpaceInvaders
