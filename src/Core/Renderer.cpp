#include "Core/Renderer.h"
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
