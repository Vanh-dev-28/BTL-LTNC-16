#include "Core/Window.h"
#include <SDL3/SDL_video.h>

namespace SpaceInvaders
{

    Window::~Window()
    {
        destroy();
    }

    bool Window::create(const std::string &title, int width, int height)
    {
        destroy();
        window_ = SDL_CreateWindow(title.c_str(), width, height, 0);
        return window_ != nullptr;
    }

    void Window::destroy()
    {
        if (window_ != nullptr)
        {
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        }
    }

    void Window::setSize(int width, int height)
    {
        if (window_ == nullptr)
        {
            return;
        }
        SDL_SetWindowSize(window_, width, height);
        center();
    }
    void Window::center()
{
    if (window_ == nullptr)
    {
        return;
    }

    SDL_DisplayID display = SDL_GetDisplayForWindow(window_);

    const SDL_DisplayMode* mode =
        SDL_GetCurrentDisplayMode(display);

    if (mode == nullptr)
    {
        return;
    }

    int windowWidth;
    int windowHeight;

    SDL_GetWindowSize(window_,
                      &windowWidth,
                      &windowHeight);

    SDL_SetWindowPosition(
        window_,
        (mode->w - windowWidth) / 2,
        (mode->h - windowHeight) / 2);
}

    SDL_Window *Window::getSDLWindow() const
    {
        return window_;
    }

} // namespace SpaceInvaders
