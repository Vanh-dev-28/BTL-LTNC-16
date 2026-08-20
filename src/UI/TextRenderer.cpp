#include "UI/TextRenderer.h"

#include "Managers/FontManager.h"
#include "Core/Renderer.h"

#include <SDL3_ttf/SDL_ttf.h>

namespace SpaceInvaders
{
    bool TextRenderer::drawText(Renderer& renderer, const std::string& fontId, const std::string& text, int x, int y, SDL_Color color)
    {
        TTF_Font* font = FontManager::instance().getFont(fontId);
        if (font == nullptr)
            return false;
        
        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), text.length(), color);
        if (surface == nullptr)
            return false;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer.getSDLRenderer(), surface);
        SDL_FRect dst = { (float)x, (float)y, (float)surface->w, (float)surface->h };
        SDL_DestroySurface(surface);
        SDL_RenderTexture(renderer.getSDLRenderer(), texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
        
        return true;
    }
}