#include "Core/Renderer.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
namespace SpaceInvaders
{

    Renderer::~Renderer()
    {
        destroy();
    }

    bool Renderer::create(SDL_Window *window)
    {
        destroy();
        renderer_ = SDL_CreateRenderer(window, nullptr);
        return renderer_ != nullptr;
    }

    void Renderer::clear()
    {
        if (renderer_ == nullptr)
        {
            return;
        }
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
        SDL_RenderClear(renderer_);
    }

    void Renderer::present()
    {
        if (renderer_ != nullptr)
        {
            SDL_RenderPresent(renderer_);
        }
    }

    void Renderer::drawText(
    const std::string& text,
    TTF_Font* font,
    SDL_Color color,
    int x,
    int y)
{
    if (renderer_ == nullptr || font == nullptr)
        return;

    SDL_Surface* surface =
        TTF_RenderText_Blended(
            font,
            text.c_str(),
            text.length(),
            color);

    if (surface == nullptr)
        return;

    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(renderer_, surface);

    if (texture == nullptr)
    {
        SDL_DestroySurface(surface);
        return;
    }

    SDL_FRect dst{
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(surface->w),
        static_cast<float>(surface->h)
    };

    SDL_DestroySurface(surface);

    SDL_RenderTexture(renderer_, texture, nullptr, &dst);

    SDL_DestroyTexture(texture);
}

    void Renderer::destroy()
    {
        if (renderer_ != nullptr)
        {
            SDL_DestroyRenderer(renderer_);
            renderer_ = nullptr;
        }
    }

    SDL_Renderer *Renderer::getSDLRenderer() const
    {
        return renderer_;
    }

} // namespace SpaceInvaders
