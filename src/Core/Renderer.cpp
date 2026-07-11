#include "Core/Renderer.h"

namespace SpaceInvaders {

Renderer::~Renderer()
{
    destroy();
}

bool Renderer::create(SDL_Window* window)
{
    destroy();
    renderer_ = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer_ == nullptr) {
        renderer_ = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }
    return renderer_ != nullptr;
}

void Renderer::clear()
{
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);
}

void Renderer::present()
{
    SDL_RenderPresent(renderer_);
}

void Renderer::destroy()
{
    if (renderer_ != nullptr) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
}

SDL_Renderer* Renderer::getSDLRenderer() const
{
    return renderer_;
}

} // namespace SpaceInvaders
