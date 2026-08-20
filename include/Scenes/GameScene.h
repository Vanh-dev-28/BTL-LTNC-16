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

    private:
        // ----------------------------- GameState Core ---------------------------------------
        enum class GameState
        {
            EnterName,
            Playing,
            EndGame
        };

        Player player_{};
        GameState gameState_{GameState::EnterName};
        bool gameOver_{};
        bool playerWon_{};
        int currentWave_{0};
        int score_{};

        std::string playerName_{};
        bool scoreSaved_{false};
        bool paused_{false};

        // ----------------------------- Enemies Wave -------------------------------------------
        void resetWave();
        bool allEnemiesDefeated() const;
        void createFlyByPreview();
        void updatePreviewEnemies(float deltaTime);

        std::vector<Enemy> enemies_{};
        std::vector<EnemySpawnData> pendingEnemies_;
        std::vector<Enemy> m_previewEnemies;
        Vector2 m_formationCenter{};
        float spawnTimer_{0.0f};
        float enemyDirection_{};
        float m_swarmSharedTime{0.0f};
        float m_diveAttackTimer{5.0f};
        float m_formationScale{1.0f};
        float m_formationScaleDirection{1.0f};
        bool inWaveTransition_{false};
        float waveTransitionTimer_{0.0f};

        //------------------------------------ Combat--------------------------------------------
        std::vector<Bullet> bullets_{};
        float enemyFireCooldown_{};
        void updateBullets(float deltaTime);
        void updateEnemies(float deltaTime);
        void checkCollisions();

        //------------------------------------ Power Up--------------------------------------------
        void updatePowerUps(float deltaTime);
        void checkPowerUpCollisions();
        void spawnPowerUp(float x, float y);
        void activatePowerUp(PowerUpType type);
        void spawnCompanions();
        void updateCompanion(float deltaTime);
        void moveCompanion(float deltaTime);
        void checkCompanionCollision();
        void renderCompanion(Renderer &renderer);

        std::vector<PowerUp> powerUps_;
        std::vector<CompanionShip> companions_;
        bool coneShotActive_{false};
        float coneShotTimer_{0.0f};
        static constexpr float CONE_SHOT_DURATION = 8.0f;

        //-------------------------------------UI--------------------------------------------------
        void render(Renderer &renderer) override;
        void updateEnterName();
        void renderEnterName(Renderer &renderer);
        void updatePauseMenu();
        void renderPauseMenu(Renderer &renderer);
        void updateEndGame();
        void renderEndGame(Renderer &renderer);
        void saveScore();

        // Background loops
        SDL_Texture *gameplayBackground_{nullptr};
        float backgroundY1_{0.0f};
        float backgroundY2_{0.0f};
        float backgroundSpeed_{80.0f};

        int endMenuIndex_{};
        bool enteringPlayerName_{false};
        SDL_FRect replayButtonRect_{};
        SDL_FRect menuButtonRect_{};
        SDL_FRect fireballButtonRect_{};
        SDL_FRect shieldButtonRect_{};
        bool mouseWasPressed_{false};
        SDL_FRect pauseButtonRect_{};
        SDL_FRect resumeButtonRect_{};
        SDL_FRect exitPauseButtonRect_{};

        // Helpers
        SDL_FRect getPlayerHitbox() const;
        SDL_FRect getPlayerShieldHitbox() const;
        SDL_FRect getEnemyLaserHitbox(const Bullet &bullet) const;
    };

} // namespace SpaceInvaders