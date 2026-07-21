#include "Core/Renderer.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
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
    void Renderer::drawTextCentered(
        const std::string& text,
        TTF_Font* font,
        SDL_Color color,
        int centerX,
        int y)
    {
    if (font == nullptr)
    {
        return;
    }

    int width = 0;
    int height = 0;

    if (!measureText(text, font, width, height))
    {
        return;
    }

    drawText(
        text,
        font,
        color,
        centerX - width / 2,
        y);
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
    void Renderer::drawTexture(
    SDL_Texture* texture,
    float x,
    float y,
    float width,
    float height)
{
    if (renderer_ == nullptr || texture == nullptr)
    {
        return;
    }

    SDL_FRect dstRect{
        x,
        y,
        width,
        height
    };

    SDL_RenderTexture(renderer_, texture, nullptr, &dstRect);
}
    bool Renderer::measureText(
        const std::string& text,
        TTF_Font* font,
        int& width,
        int& height) const
    {
        if (font == nullptr)
        {
        return false;
        }

        return TTF_GetStringSize(
        font,
        text.c_str(),
        text.length(),
        &width,
        &height);
    }
    void Renderer::fillRect(float x, float y, float width, float height, SDL_Color color) {
    SDL_FRect rect{ x, y, width, height };

    SDL_SetRenderDrawColor(renderer_,
                           color.r,
                           color.g,
                           color.b,
                           color.a);

    SDL_RenderFillRect(renderer_, &rect); 
    }

    void Renderer::drawRect(float x, float y, float width, float height, SDL_Color color) {
    SDL_FRect rect{ x, y, width, height };

    SDL_SetRenderDrawColor(renderer_,
                           color.r,
                           color.g,
                           color.b,
                           color.a);

    SDL_RenderRect(renderer_, &rect);
    }

    void Renderer::drawLine(
    float x1,
    float y1,
    float x2,
    float y2,
    SDL_Color color)
{
    SDL_SetRenderDrawColor(
        renderer_,
        color.r,
        color.g,
        color.b,
        color.a);

    SDL_RenderLine(
        renderer_,
        x1,
        y1,
        x2,
        y2);
}
} 