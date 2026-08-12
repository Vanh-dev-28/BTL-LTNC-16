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
        void updateEndGame();
        void renderEndGame(Renderer &renderer);

        Player player_{};

        float enemyDirection_{};
        float enemyFireCooldown_{};
        int currentWave_{0};
        int score_{};
        bool gameOver_{};
        bool playerWon_{};
        bool inWaveTransition_{false};
        float waveTransitionTimer_{0.0f};
        int endMenuIndex_{};
        std::vector<Bullet> bullets_{};
        std::vector<Enemy> enemies_{};

        // UI and Abilities
        SDL_FRect fireballButtonRect_{};
        SDL_FRect shieldButtonRect_{};
        bool mouseWasPressed_{false};
    };

}