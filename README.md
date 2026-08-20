# SPACE INVADERS - C++ / SDL3

**Trò chơi Bắn máy bay 2D:** Người chơi điều khiển chiến đấu cơ của mình để tiêu diệt tất cả kẻ địch ngoài hành tinh.

---

## 1. Mô tả bài toán và phạm vi

**Mục tiêu trò chơi:**
Xây dựng trò chơi *Space Invaders* trên nền tảng máy tính sử dụng ngôn ngữ lập trình **C++** và thư viện **SDL3**. 
- Người chơi sẽ sử dụng chuột để di chuyển phi thuyền trong phạm vi màn hình và khai hỏa tiêu diệt kẻ địch. 
- Nhiệm vụ chính là sinh tồn, vượt qua tất cả các đợt tấn công của quái vật để giành chiến thắng và đạt được điểm số cao nhất.

**Hệ thống tính năng :**
- **Gameplay:** Cơ chế bắn súng, di chuyển mượt mà.
- **Menu:** Giao diện điều hướng chính.
- **Âm thanh & Hình ảnh:** Hiệu ứng sống động.
- **Phông chữ:** Hỗ trợ hiển thị text tùy chỉnh.
- **Cài đặt (Settings):** Tùy chỉnh thông số trò chơi.
- **Bảng xếp hạng (Ranking):** Lưu trữ thành tích.

---

## 2. Công nghệ, Môi trường & Yêu cầu cài đặt

### Công nghệ sử dụng
- **Ngôn ngữ lập trình:** `C++`
- **Thư viện đồ họa & Đa phương tiện:** `SDL3`, `SDL3_image`, `SDL3_ttf`, `SDL3_mixer`
- **Công cụ Build:** Sử dụng `CMake` (CMakeLists) để tự động hóa quá trình biên dịch và tạo file thực thi `.exe`.

### Môi trường phát triển
- **Hệ điều hành:** Windows 10 / 11
- **IDE:** Visual Studio (hoặc các trình soạn thảo tương đương)
- **Môi trường biên dịch:** MSYS2 + UCRT64 (Cung cấp compiler và MinGW)
- **Trình biên dịch:** `G++`

### Yêu cầu cài đặt (Dành cho người dùng / Developer)
Để có thể clone dự án về, tự biên dịch và chạy trò chơi, máy tính của bạn cần đáp ứng các yêu cầu sau:
- Sử dụng hệ điều hành **Windows 10 / 11**.
- Trình biên dịch hỗ trợ chuẩn **C++17** trở lên.
- Đã cài đặt **MinGW-w64** hoặc bộ **MSYS2 + UCRT64**.
- Đã cài đặt **CMake** (phiên bản 4.x trở lên).
- Đã cài đặt **Git** để tải source code.

---

## 3. Cấu trúc thư mục và các module chính

```text
SpaceInvaders/
│
├── assets/             # Hình ảnh, âm thanh, font chữ trò chơi sử dụng
│
├── include/            # Các file header dùng để khai báo lớp, hàm
│   ├── Core/            # Bao gồm lớp Game chính và các lớp tính năng
│   ├── Entities/        # Bao gồm các lớp của đối tượng cụ thể
│   ├── Managers/        # Bao gồm các lớp quản lí đối tượng, chương trình
│   ├── Scenes/          # Bao gồm các lớp giao diện
│   └── Utils/           # Lớp đối tượng ngoài cho entities
|   └── UI/              # Lớp đối tượng ngoài cho giao diẹn
│
├── src/                # Các file mã nguồn chứa logic gameloop và workflow của các Scene
│   ├── Core/           # Chức năng của lớp Game chính và các lớp tính năng
│   ├── Entities/       # Chức năng và phạm vi thực hiện của các đối tượng
│   ├── Managers/       # Quản lí tài nguyên và hệ thống dùng chung
│   ├── Scenes/         # Luồng xử lí của các Scene
│
├── third_party/        # Các thư viện đồ họa SDL3
│
├── CMakeLists.txt      # Cấu hình build bằng CMake
├── Makefile            # Cấu hình build bằng Make
├── main.cpp            # Điểm bắt đầu chương trình
└── README.md           # Tài liệu hướng dẫn project
```

### Các module chính

* **Core:** Quản lý Game Loop, cửa sổ, renderer, input và timer.
* **Entities:** Quản lý các đối tượng trong game.
* **Managers:** Quản lý tài nguyên và các hệ thống dùng chung.
* **Scenes:** Quản lý menu và gameplay.
* **Utils:** Chứa hằng số và các tiện ích dùng chung.
* **assets:** Chứa tài nguyên hình ảnh, âm thanh và font.
* **third_party:** Chứa các thư viện bên ngoài được project sử dụng.

## 4. Cách chạy chương trình

#### Bước 1 — Cài đặt Visual Studio Code

Tải và cài đặt Visual Studio Code:

https://code.visualstudio.com/

Sau khi cài đặt, mở VS Code và cài các extension:

* **C/C++** — hỗ trợ viết, biên dịch và debug C++.
* **CMake Tools** — hỗ trợ cấu hình và build project bằng CMake.

---

#### Bước 2 — Cài đặt MSYS2 và trình biên dịch C++

Tải MSYS2 tại:

https://www.msys2.org/

Sau khi cài đặt, mở **MSYS2 UCRT64**.

Cập nhật hệ thống:

```bash
pacman -Syu
```

Nếu MSYS2 yêu cầu đóng và mở lại terminal, hãy thực hiện theo hướng dẫn rồi tiếp tục chạy lệnh cập nhật.

Cài trình biên dịch C++ và các công cụ build:

```bash
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
```

Kiểm tra compiler:

```bash
g++ --version
```

Nếu terminal hiển thị phiên bản của `g++` thì trình biên dịch đã được cài đặt thành công.

---

#### Bước 3 — Cài đặt CMake

CMake được sử dụng để tạo hệ thống build cho project.

Tải CMake tại:

https://cmake.org/download/

Sau khi cài đặt, kiểm tra:

```bash
cmake --version
```

Nếu terminal hiển thị phiên bản CMake thì quá trình cài đặt đã thành công.

---

#### Bước 4 — Tải mã nguồn của project

Clone repository của nhóm:

```bash
git clone <https://github.com/Vanh-dev-28/BTL-LTNC-16>
```

Sau đó đi vào thư mục project:

```bash
cd SpaceInvaders
```

Hoặc có thể chọn **Code → Download ZIP** trên GitHub rồi giải nén project.

Sau khi tải về, cần đảm bảo project có các thư mục chính:

```text
SpaceInvaders/
├── assets/
├── include/
├── src/
├── third_party/
├── CMakeLists.txt
└── main.cpp
```

---

#### Bước 5 — Tạo thư mục Build

Mở terminal **MSYS2 UCRT64** tại thư mục gốc của project và chạy:

```bash
cmake -S . -B build -G "MinGW Makefiles"
```

Lệnh trên sẽ tạo thư mục:

```text
build/
```

Thư mục `build` là thư mục được CMake tạo ra để chứa các file trung gian và file chương trình sau khi biên dịch.

Sau bước này, project có dạng:

```text
SpaceInvaders/
├── assets/
├── include/
├── src/
├── third_party/
├── build/
├── CMakeLists.txt
└── main.cpp
```

#### Bước  — Build chương trình

Sau khi CMake cấu hình thành công, chạy:

```bash
cmake --build build
```

CMake sẽ biên dịch các file `.cpp`, liên kết với các thư viện SDL3 và tạo file thực thi của game.

Nếu quá trình build thành công, file:

```text
SpaceInvaders.exe
```

sẽ được tạo trong thư mục build hoặc vị trí được cấu hình bởi `CMakeLists.txt`.

---

#### Bước 7 — Chạy chương trình

Có thể chạy trực tiếp từ thư mục gốc:

```bash
.\build\SpaceInvaders.exe
```

Hoặc đi vào thư mục build:

```bash
cd build
```

sau đó:

```bash
.\SpaceInvaders.exe
```

Game sẽ khởi động và hiển thị **Main Menu**.

---

## 5. Các chức năng chính

- Menu chính: Start Game, Ranking, Settings, Exit.
- Nhập tên người chơi trước khi bắt đầu game.
- Điều khiển Player và bắn đạn.
- Hệ thống Enemy với nhiều loại kẻ địch và kiểu di chuyển khác nhau.
- Hệ thống Wave với nhiều màn chơi và chuyển Wave.
- Hệ thống chiến đấu: đạn, va chạm, tiêu diệt Enemy và tính điểm.
- Hệ thống Power-up: hồi máu, đạn 3 nòng, triệu hồi máy bay chiến đấu  và tự động bắn hỗ trợ Player.
- Hệ thống Skills: Bắn fireball và khiên
- Pause Game: tạm dừng và tiếp tục trò chơi.
- Game Over và Victory: Trạng thái thắng/thua, chơi lại hoặc quay về Menu.
- Ranking: lưu và hiển thị Top 10 điểm số người chơi cao nhất.
- Settings: thay đổi độ phân giải và âm lượng.
- Hệ thống âm thanh: nhạc nền và hiệu ứng âm thanh.
- Đồ họa và giao diện: background scrolling, font, popups ...

## 6. Báo cáo PDF về dự án: https://drive.google.com/file/d/1L0Ty05rmDmXpbqHjjQUvxJENT3MMFf6N/view?usp=drive_link

## 7. Video DEMO sản phẩm trò chơi: https://drive.google.com/drive/u/0/folders/1I6WVo1QEYd4UlizHyd4Oid4cjqM78Lsd
