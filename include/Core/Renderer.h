#pragma once

#include <SDL3/SDL.h>

namespace SpaceInvaders {

/** Owns the SDL renderer and exposes frame-level rendering operations. */
class Renderer {
public:
    Renderer() = default;
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool create(SDL_Window* window);
    void clear();
    void present();
    void destroy();
    [[nodiscard]] SDL_Renderer* getSDLRenderer() const;

private:
    SDL_Renderer* renderer_ {nullptr};
};

} // namespace SpaceInvaders
