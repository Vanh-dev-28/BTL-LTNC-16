#pragma once

#include "Scenes/Scene.h"

namespace SpaceInvaders
{
    class RankingScene : public Scene
    {
        public:
            RankingScene() = default;
            ~RankingScene() override = default;
            void enter() override;
            void exit() override;
            void update(float deltaTime) override;
            void render(Renderer& renderer) override;
    };
} //namespace SpaceInvaders