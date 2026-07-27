#pragma once

#include "Core/Renderer.h"

namespace SpaceInvaders
{

    class Enemy
    {
    public:
        Enemy() = default;
        ~Enemy() = default;

        // Khởi tạo vị trí
        void spawn(float x, float y);

        // Cập nhật mỗi frame
        void update(float deltaTime);

        // Vẽ lên màn hình
        void render(Renderer &renderer);

        // Bị tiêu diệt
        void destroy();

        // Kiểm tra còn sống không
        bool isAlive() const;

        // Lấy vị trí
        float getX() const;
        float getY() const;

    private:
        float x_{};
        float y_{};

        float speed_{150.0f};

        bool alive_{true};
    };

}