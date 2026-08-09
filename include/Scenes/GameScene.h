#pragma once

#include "Scenes/Scene.h"
#include <vector>
#include "Entities/Player.h"
#include "Entities/Enemy.h"
#include "Entities/Bullet.h"

namespace SpaceInvaders
{

    class GameScene : public Scene
    {
    public:
        GameScene() = default;
        ~GameScene() override = default;

        void enter() override;
        void exit() override;

        void update(float deltaTime) override;
        void render(Renderer &renderer) override;

    private:
        void updateBullets(float deltaTime);
        void updateEnemies(float deltaTime);
        void checkCollisions();
        bool allEnemiesDefeated() const;

        void resetWave();

        Player player_{};

        float enemyDirection_{};
        int score_{};
        bool gameOver_{};
        std::vector<Bullet> bullets_{};
        std::vector<Enemy> enemies_{};
    };

}