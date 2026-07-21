#include <Core/Game.h>
#include <SDL3/SDL.h>
#include <iostream>
#include <windows.h>

int main(int /*argc*/, char * /*argv*/[])
{
    std::cout << "Initializing SDL..." << std::endl;
    int sdlVersion = SDL_GetVersion();
    std::cout << "Linked SDL version: " << (sdlVersion / 1000000) << "." << ((sdlVersion / 1000) % 1000) << "." << (sdlVersion % 1000) << std::endl;
    std::cout << "Linked SDL revision: " << (SDL_GetRevision() ? SDL_GetRevision() : "(none)") << std::endl;

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
