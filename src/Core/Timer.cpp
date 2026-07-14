#include "Core/Timer.h"

namespace SpaceInvaders
{

    void Timer::update()
    {
        const Uint64 currentCounter = SDL_GetPerformanceCounter();
        if (lastCounter_ != 0U)
        {
            const Uint64 elapsed = currentCounter - lastCounter_;
            deltaTime_ = static_cast<float>(elapsed) / static_cast<float>(SDL_GetPerformanceFrequency());
            fps_ = deltaTime_ > 0.0F ? 1.0F / deltaTime_ : 0.0F;
        }
        lastCounter_ = currentCounter;
    }

    float Timer::deltaTime() const { return deltaTime_; }
    float Timer::fps() const { return fps_; }

} // namespace SpaceInvaders
