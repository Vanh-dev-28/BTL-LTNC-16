#pragma once

#include <SDL3/SDL.h>
#include <string>

namespace SpaceInvaders {

/** Owns the native SDL window used by the game. */
class Window {
public:
    Window() = default;
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool create(const std::string& title, int width, int height);
    void destroy();
    [[nodiscard]] SDL_Window* getSDLWindow() const;

private:
    SDL_Window* window_ {nullptr};
};

} // namespace SpaceInvaders
