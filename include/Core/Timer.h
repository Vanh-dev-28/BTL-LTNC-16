#pragma once

#include <SDL3/SDL.h>

namespace SpaceInvaders {

/** Measures frame duration and exposes a smoothed frame rate value. */
class Timer {
public:
    void update();
    [[nodiscard]] float deltaTime() const;
    [[nodiscard]] float fps() const;

private:
    Uint64 lastCounter_ {0U};
    float deltaTime_ {0.0F};
    float fps_ {0.0F};
};

} // namespace SpaceInvaders
