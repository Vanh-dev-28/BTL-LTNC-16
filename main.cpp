#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

struct Player {
    float x, y;
    float width, height;
    float speed;
};

struct Bullet {
    float x, y;
    float width, height;
    float speed;
    bool active;
};

struct Enemy {
    float x, y;
    float width, height;
    float speedX;
    bool active;
};

bool CheckCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return (x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2);
}

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Chicken Invaders Prototype", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        std::cerr << "CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Khởi tạo seed random
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Player player = { WINDOW_WIDTH / 2.0f - 25.0f, WINDOW_HEIGHT - 60.0f, 50.0f, 40.0f, 400.0f };
    
    std::vector<Bullet> bullets;
    float bulletCooldown = 0.0f;

    std::vector<Enemy> enemies;
    int rows = 4;
    int cols = 8;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            enemies.push_back({
                c * 70.0f + 100.0f,
                r * 50.0f + 50.0f,
                40.0f, 35.0f,
                100.0f,
                true
            });
        }
    }

    bool running = true;
    SDL_Event event;
    Uint64 lastTime = SDL_GetTicks();

    while (running) {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        const bool* state = SDL_GetKeyboardState(nullptr);

        // Di chuyển người chơi
        if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]) {
            player.x -= player.speed * deltaTime;
        }
        if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]) {
            player.x += player.speed * deltaTime;
        }

        // Giới hạn người chơi trong màn hình
        if (player.x < 0) player.x = 0;
        if (player.x + player.width > WINDOW_WIDTH) player.x = WINDOW_WIDTH - player.width;

        // Bắn đạn
        if (bulletCooldown > 0.0f) {
            bulletCooldown -= deltaTime;
        }
        if (state[SDL_SCANCODE_SPACE] && bulletCooldown <= 0.0f) {
            bullets.push_back({ player.x + player.width / 2.0f - 5.0f, player.y, 10.0f, 20.0f, 600.0f, true });
            bulletCooldown = 0.2f; // Bắn mỗi 0.2 giây
        }

        // Cập nhật vị trí đạn
        for (auto& b : bullets) {
            if (b.active) {
                b.y -= b.speed * deltaTime;
                if (b.y < 0) b.active = false;
            }
        }

        // Di chuyển địch (gà)
        bool reverseDir = false;
        for (auto& e : enemies) {
            if (e.active) {
                e.x += e.speedX * deltaTime;
                // Kiểm tra va chạm với cạnh viền
                if (e.x <= 0 || e.x + e.width >= WINDOW_WIDTH) {
                    reverseDir = true;
                }
            }
        }

        // Đổi hướng và tiến xuống khi đụng tường
        if (reverseDir) {
            for (auto& e : enemies) {
                if (e.active) {
                    e.speedX = -e.speedX;
                    e.y += 20.0f; 
                }
            }
        }

        // Xử lý va chạm đạn & địch
        for (auto& b : bullets) {
            if (!b.active) continue;
            for (auto& e : enemies) {
                if (!e.active) continue;

                if (CheckCollision(b.x, b.y, b.width, b.height, e.x, e.y, e.width, e.height)) {
                    b.active = false;
                    e.active = false;
                    break;
                }
            }
        }

        // Dọn dẹp đạn không hoạt động
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }), bullets.end());

        // Kiểm tra điều kiện thắng (hoặc respawn wave mới)
        bool allDead = true;
        for (const auto& e : enemies) {
            if (e.active) {
                allDead = false;
                break;
            }
        }
        if (allDead) {
            enemies.clear();
            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    enemies.push_back({
                        c * 70.0f + 100.0f,
                        r * 50.0f + 50.0f,
                        40.0f, 35.0f,
                        100.0f, // Tốc độ giữ nguyên
                        true
                    });
                }
            }
        }

        // Render Game
        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255); // Background xanh đậm không gian
        SDL_RenderClear(renderer);

        // Vẽ Player (Tàu vũ trụ hình chữ nhật)
        SDL_FRect playerRect = { player.x, player.y, player.width, player.height };
        SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
        SDL_RenderFillRect(renderer, &playerRect);

        // Vẽ Đạn (Hình chữ nhật vàng)
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for (const auto& b : bullets) {
            if (b.active) {
                SDL_FRect bRect = { b.x, b.y, b.width, b.height };
                SDL_RenderFillRect(renderer, &bRect);
            }
        }

        // Vẽ Kẻ địch (Gà - hình chữ nhật đỏ/cam)
        SDL_SetRenderDrawColor(renderer, 255, 80, 50, 255);
        for (const auto& e : enemies) {
            if (e.active) {
                SDL_FRect eRect = { e.x, e.y, e.width, e.height };
                SDL_RenderFillRect(renderer, &eRect);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
