cmake_minimum_required(VERSION 3.16)

project(SpaceInvadersEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

add_executable(SpaceInvaders
    main.cpp
    src/Core/Game.cpp
    src/Core/Input.cpp
    src/Core/Renderer.cpp
    src/Core/Timer.cpp
    src/Core/Window.cpp
    src/Entities/Entity.cpp
    src/Managers/AudioManager.cpp
    src/Managers/FontManager.cpp
    src/Managers/SceneManager.cpp
    src/Managers/TextureManager.cpp
    src/Scenes/Scene.cpp
    src/UI/Widget.cpp
    src/Utils/Constants.cpp
    src/Utils/Vector2.cpp
)

target_include_directories(SpaceInvaders PRIVATE include)

target_link_libraries(SpaceInvaders PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
)

if(MSVC)
    target_compile_options(SpaceInvaders PRIVATE /W4 /permissive-)
else()
    target_compile_options(SpaceInvaders PRIVATE -Wall -Wextra -Wpedantic)
endif()
