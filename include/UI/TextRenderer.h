#pragma once

#include <string>
#include <SDL3/SDL.h>

namespace SpaceInvaders
{
    class Renderer;
    class TextRenderer
    {
    public:
        static bool drawText(Renderer& renderer, const std::string& fontId, const std::string& text, int x, int y, SDL_Color color);
    };
}