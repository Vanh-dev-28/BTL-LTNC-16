#pragma once

#include <SDL_mixer.h>
#include <string>
#include <unordered_map>

namespace SpaceInvaders {

/** Provides music and sound-effect playback services. */
class AudioManager {
public:
    static AudioManager& instance();

    bool playMusic(const std::string& path, int loops = -1);
    bool playSFX(const std::string& path, int loops = 0);
    void stopMusic();
    void clear();

private:
    AudioManager() = default;
    ~AudioManager();

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    Mix_Music* currentMusic_ {nullptr};
    std::unordered_map<std::string, Mix_Chunk*> soundEffects_;
};

} // namespace SpaceInvaders
