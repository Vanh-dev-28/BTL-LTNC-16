#pragma once

#include "Scenes/Scene.h"
#include <vector>
#include <string>
#include "Entities/Player.h"
#include "Entities/Enemy.h"
#include "Entities/Bullet.h"
#include "Entities/PowerUp.h"
#include "Entities/CompanionShip.h"
#include <SDL3/SDL.h>

namespace SpaceInvaders
{
    // Data structure for defining an enemy to be spawned.
    // This allows for more complex wave designs with per-enemy spawn delays.
    struct EnemySpawnData
    {
        EnemyType type;
        EnemyMovementPattern movePattern;
        EnemyEntryPattern entryPattern;

        Vector2 startPos;
        Vector2 targetPos;

        float speed;

        Vector2 c1 = {0, 0};
        Vector2 c2 = {0, 0};

        float spawnDelay = 0.1f; // Default spawn delay, matches old spawnInterval_
    };

    class GameScene : public Scene
    {
    public:
        GameScene() = default;
        GameScene(const std::string &playerName);
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
        //Background loops
        SDL_Texture* gameplayBackground_{nullptr};
        float backgroundY1_{0.0f};
        float backgroundY2_{0.0f};
        float backgroundSpeed_{80.0f};
        void updateBullets(float deltaTime);
        void updateEnemies(float deltaTime);
        void checkCollisions();
        bool allEnemiesDefeated() const;

        void resetWave();
        void updateEnterName();
        void updateEndGame();

        // Task F: Preview enemies
        void createFlyByPreview();
        void updatePreviewEnemies(float deltaTime);

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
        // input: getplayername.
        bool enteringPlayerName_{false};
        std::string playerName_{};

        // --- New Gameplay State (Tasks B, C, D, E) ---
        float m_swarmSharedTime{0.0f};
        Vector2 m_formationCenter{};
        // Dive attack
        float m_diveAttackTimer{5.0f};
        // Expansion/Contraction
        float m_formationScale{1.0f};
        float m_formationScaleDirection{1.0f};

        // --- Wave Transition (Task F) ---
        std::vector<Enemy> m_previewEnemies;

        SDL_FRect replayButtonRect_{};
        SDL_FRect menuButtonRect_{};
        std::vector<Bullet> bullets_{};
        std::vector<Enemy> enemies_{};

        // Wave spawning queue
        std::vector<EnemySpawnData> pendingEnemies_;
        float spawnTimer_{0.0f};

        // PowerUp
        std::vector<PowerUp> powerUps_;
        std::vector<CompanionShip> companions_;
        bool coneShotActive_{false};
        float coneShotTimer_{0.0f};
        static constexpr float CONE_SHOT_DURATION = 8.0f;
        void updatePowerUps(float deltaTime);
        void checkPowerUpCollisions();
        void spawnPowerUp(float x, float y);
        void activatePowerUp(PowerUpType type);

        // UI and Abilities
        SDL_FRect fireballButtonRect_{};
        SDL_FRect shieldButtonRect_{};
        bool mouseWasPressed_{false};

        // ScoreSaved state
        bool scoreSaved_{false};

        // pause game
        bool paused_{false};
        SDL_FRect pauseButtonRect_{};
        SDL_FRect resumeButtonRect_{};
        SDL_FRect exitPauseButtonRect_{};
        void updatePauseMenu();

        void renderPauseMenu(Renderer &renderer);
        // powerUp companion
        void updateCompanion(float deltaTime);
        void renderCompanion(Renderer &renderer);
        void checkCompanionCollision();
        void moveCompanion(float deltaTime);
        void spawnCompanions();
    };

}