#include <Core/Game.h>
#include <SDL3/SDL.h>
#include <iostream>
#include <windows.h>

int main(int /*argc*/, char * /*argv*/[])
{
    // Test SDL_Init
    std::cout << "Initializing SDL..." << std::endl;
    // Try initializing subsystems individually to pinpoint failures
    // Log linked SDL version and revision
    int sdlVersion = SDL_GetVersion();
    std::cout << "Linked SDL version: " << (sdlVersion / 1000000) << "." << ((sdlVersion / 1000) % 1000) << "." << (sdlVersion % 1000) << std::endl;
    std::cout << "Linked SDL revision: " << (SDL_GetRevision() ? SDL_GetRevision() : "(none)") << std::endl;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    if (!SDL_CreateWindowAndRenderer("Space Invaders", 800, 600, 0, &window, &renderer))
    {
        SDL_Log("Khởi tạo cửa sổ thất bại: %s", SDL_GetError());
        return -1;
    }
    std::cout << "SDL initialized successfully" << std::endl;

    SpaceInvaders::Game game;
    if (!game.initialize())
    {
        const char *error = SDL_GetError();
        std::cerr << "Game initialization failed: " << error << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                 "Error",
                                 error,
                                 NULL);
        return 1;
    }

    game.run();
    return 0;
}
