#pragma once

#include "Core/Input.h"
#include "Core/Renderer.h"
#include "Core/Timer.h"
#include "Core/Window.h"
#include "Managers/SceneManager.h"

namespace SpaceInvaders
{

    /** Coordinates SDL subsystems and the main application loop. */
    /**
     * Lớp Game: Đóng vai trò điều hành chính, phối hợp các hệ thống con của SDL
     * và quản lý vòng lặp chính (Game Loop) của ứng dụng.
     */
    class Game
    {
    public:
        // --- CONSTRUCTORS & DESTRUCTOR ---
        Game() = default; // Hàm khởi tạo mặc định
        ~Game();          // Hàm hủy: Dùng để giải phóng tài nguyên khi game kết thúc

        // Xóa Copy Constructor và Copy Assignment Operator để đảm bảo
        // chỉ có duy nhất 1 đối tượng Game tồn tại (không bị sao chép nhầm)
        Game(const Game &) = delete;
        Game &operator=(const Game &) = delete;

        // --- CÁC PHƯƠNG THỨC VÒNG LẶP GAME (GAME LOOP METHODS) ---

        /** Khởi tạo SDL, tạo cửa sổ, renderer và các hệ thống phụ trợ. Trả về true nếu thành công. */
        bool initialize();

        /** Bắt đầu chạy vòng lặp game chính (Game Loop: Xử lý sự kiện -> Update -> Render). */
        void run();

        /** Lắng nghe và xử lý các sự kiện đầu vào từ người dùng (bàn phím, chuột, nút đóng cửa sổ). */
        void handleEvents();

        /** Cập nhật logic game (vị trí nhân vật, thời gian, va chạm, trạng thái màn chơi). */
        void update();

        /** Vẽ (hiển thị) tất cả các hình ảnh/đối tượng game lên màn hình. */
        void render();

        /** Dọn dẹp tài nguyên, đóng SDL và thoát game an toàn. */
        void clean();

        void applySettings();

        // --- CÁC HÀM GETTER / TRUY XUẤT TRẠNG THÁI ---

        /** Kiểm tra xem game có đang chạy hay không. */
        [[nodiscard]] bool isRunning() const;

        /** Trả về tham chiếu tới đối tượng quản lý đầu vào (phím/chuột) để các lớp khác sử dụng. */
        [[nodiscard]] Input &input();

        /** Trả về tham chiếu tới đối tượng quản lý chuyển cảnh (chuyển giữa Menu, Game Play, Game Over). */
        [[nodiscard]] SceneManager &scenes();

    private:
        // --- THUỘC TÍNH / TRẠNG THÁI (PROPERTIES) ---

        /** Cờ kiểm tra xem vòng lặp game có đang chạy không (true = đang chạy, false = dừng). */
        bool running_{false};

        /** Cờ kiểm tra xem game đã khởi tạo thành công chưa. */
        bool initialized_{false};

        /** Quản lý cửa sổ hiển thị của game (kích thước, tiêu đề). */

        Window window_;

        /** Quản lý việc vẽ/hiển thị đồ họa (SDL_Renderer). */
        Renderer renderer_;

        /** Quản lý việc đọc tín hiệu từ bàn phím/chuột. */
        Input input_;

        /** Quản lý thời gian, tính toán Delta Time (thời gian trôi giữa các khung hình). */
        Timer timer_;

        /** Quản lý các màn chơi / cảnh trong game (Menu, Play, End Game). */
        SceneManager sceneManager_;
    };

} // namespace SpaceInvaders