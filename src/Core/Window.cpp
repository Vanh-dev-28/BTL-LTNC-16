#include "Core/Window.h"

namespace SpaceInvaders {

Window::~Window()
{
    destroy();
}

bool Window::create(const std::string& title, int width, int height)
{
    destroy();
    window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               width, height, SDL_WINDOW_SHOWN);
    return window_ != nullptr;
}

void Window::destroy()
{
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
}

SDL_Window* Window::getSDLWindow() const
{
    return window_;
}

} // namespace SpaceInvaders
