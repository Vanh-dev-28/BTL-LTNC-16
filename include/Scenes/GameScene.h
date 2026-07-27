#pragma once

#include "Scenes/Scene.h"
#include <vector>

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
        struct Bullet
        {
            float x{};
            float y{};
            float speed{};
            bool active{};
        };

        struct Enemy
        {
            float x{};
            float y{};
            float speed{};
            bool alive{};
        };

        void spawnBullet();
        void updateBullets(float deltaTime);
        void updateEnemies(float deltaTime);
        void checkCollisions();
        bool allEnemiesDefeated() const;

        void resetWave();

        float playerX_{};
        float playerY_{};
        float playerSpeed_{};
        float fireCooldown_{};
        float enemyDirection_{};
        int score_{};
        bool gameOver_{};
        std::vector<Bullet> bullets_{};
        std::vector<Enemy> enemies_{};
    };

}