#include "Core/Game.h"

int main(int /*argc*/, char* /*argv*/[])
{
    SpaceInvaders::Game game;
    if (!game.initialize()) {
        return 1;
    }

    game.run();
    return 0;
}
