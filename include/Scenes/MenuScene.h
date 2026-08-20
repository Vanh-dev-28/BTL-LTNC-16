#pragma once

#include "Scenes/Scene.h"
#include <vector>
#include <string>
#include <SDL3/SDL.h>

namespace SpaceInvaders
{
    class MenuScene : public Scene
    {
        public:
            MenuScene() = default;
            ~MenuScene() override = default;
            void enter() override;
            void exit() override;
            void update(float deltaTime) override;
            void render(Renderer& renderer) override;
        private:
            SDL_FRect getMenuItemRect(int index) const;
            std::vector<std::string> menuItems_
            {
                "Start Game",
                "Ranking",
                "Settings",
                "Exit"
            };
            int selectedIndex_ = 0;
    };

}// namespace SpaceInvaders