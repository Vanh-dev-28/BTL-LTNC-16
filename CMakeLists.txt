cmake_minimum_required(VERSION 3.16)

project(SpaceInvadersEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Thêm đường dẫn tới các thư viện trong third_party
list(APPEND CMAKE_PREFIX_PATH
    "${CMAKE_SOURCE_DIR}/third_party/SDL3"
    "${CMAKE_SOURCE_DIR}/third_party/SDL3_image"
    "${CMAKE_SOURCE_DIR}/third_party/SDL3_ttf"
    "${CMAKE_SOURCE_DIR}/third_party/SDL3_mixer"
)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(SDL3_mixer CONFIG REQUIRED)

add_executable(SpaceInvaders
    main.cpp
    src/Core/Game.cpp
    src/Core/Input.cpp
    src/Core/Renderer.cpp
    src/Core/Timer.cpp
    src/Core/Window.cpp
    src/Entities/Entity.cpp
    src/Entities/Player.cpp
    src/Entities/Enemy.cpp
    src/Entities/Bullet.cpp
    src/Managers/AudioManager.cpp
    src/Managers/FontManager.cpp
    src/Managers/SceneManager.cpp
    src/Managers/TextureManager.cpp
    src/Managers/RankingManager.cpp
    src/Managers/SettingsManager.cpp
    src/Scenes/Scene.cpp
    src/Scenes/MenuScene.cpp
    src/Scenes/GameScene.cpp
    src/Scenes/RankingScene.cpp
    src/Scenes/SettingsScene.cpp
    src/UI/Widget.cpp
    src/Utils/Constants.cpp
    src/Utils/Vector2.cpp
)

target_include_directories(SpaceInvaders PRIVATE include)

target_link_libraries(SpaceInvaders PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    SDL3_mixer::SDL3_mixer
)
# Copy SDL DLLs to executable folder after build
add_custom_command(TARGET SpaceInvaders POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/third_party/SDL3/bin/SDL3.dll"
        $<TARGET_FILE_DIR:SpaceInvaders>

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/third_party/SDL3_image/bin/SDL3_image.dll"
        $<TARGET_FILE_DIR:SpaceInvaders>

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/third_party/SDL3_ttf/bin/SDL3_ttf.dll"
        $<TARGET_FILE_DIR:SpaceInvaders>

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/third_party/SDL3_mixer/bin/SDL3_mixer.dll"
        $<TARGET_FILE_DIR:SpaceInvaders>
)

if(MSVC)
    target_compile_options(SpaceInvaders PRIVATE /W4 /permissive-)
else()
    target_compile_options(SpaceInvaders PRIVATE -Wall -Wextra -Wpedantic)
endif()