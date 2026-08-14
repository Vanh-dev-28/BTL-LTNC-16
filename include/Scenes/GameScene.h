#pragma once

#include "Scenes/Scene.h"
#include <vector>
#include <string>
#include "Entities/Player.h"
#include "Entities/Enemy.h"
#include "Entities/Bullet.h"
#include <SDL3/SDL.h>

namespace SpaceInvaders
{

    class GameScene : public Scene
    {
    public:
        GameScene() = default;
        GameScene(const std::string& playerName);
        ~GameScene() override = default;

        void enter() override;
        void exit() override;

        void update(float deltaTime) override;
        void render(Renderer &renderer) override;

        void saveScore();
    private:
        enum class GameState
        {
            EnterName,
            Playing,
            EndGame
        };
        void updateBullets(float deltaTime);
        void updateEnemies(float deltaTime);
        void checkCollisions();
        bool allEnemiesDefeated() const;

        void resetWave();
        void updateEnterName();
        void updateEndGame();

        void renderEnterName(Renderer &renderer);
        void renderEndGame(Renderer &renderer);

        Player player_{};

        float enemyDirection_{};
        float enemyFireCooldown_{};
        int currentWave_{0};
        int score_{};
        GameState gameState_{GameState::EnterName};
        bool gameOver_{};
        bool playerWon_{};
        bool inWaveTransition_{false};
        float waveTransitionTimer_{0.0f};
        int endMenuIndex_{};
        //input: getplayername.
        bool enteringPlayerName_{false};
        std::string playerName_{};

        SDL_FRect replayButtonRect_{};
        SDL_FRect menuButtonRect_{};
        std::vector<Bullet> bullets_{};
        std::vector<Enemy> enemies_{};

        // UI and Abilities
        SDL_FRect fireballButtonRect_{};
        SDL_FRect shieldButtonRect_{};
        bool mouseWasPressed_{false};

        //ScoreSaved state
        bool scoreSaved_{false};

        //pause game 
        bool paused_{false};
        SDL_FRect pauseButtonRect_{};
        SDL_FRect resumeButtonRect_{};
        SDL_FRect exitPauseButtonRect_{};
        void updatePauseMenu();
        void renderPauseMenu(Renderer& renderer);
    };

}