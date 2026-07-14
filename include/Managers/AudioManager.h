#pragma once

#include <SDL3_mixer/SDL_mixer.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace SpaceInvaders
{

    /** Provides music and sound-effect playback services. */
    class AudioManager
    {
    public:
        static AudioManager &instance();

        /** Initializes SDL_mixer and opens the default playback device. */
        bool initialize();
        bool playMusic(const std::string &path, int loops = -1);
        bool playSFX(const std::string &path, int loops = 0);
        void stopMusic();
        void clear();

    private:
        AudioManager() = default;
        ~AudioManager();

        AudioManager(const AudioManager &) = delete;
        AudioManager &operator=(const AudioManager &) = delete;

        MIX_Mixer *mixer_{nullptr};
        MIX_Track *musicTrack_{nullptr};
        bool mixerLibraryInitialized_{false};
        MIX_Audio *currentMusic_{nullptr};
        std::unordered_map<std::string, MIX_Audio *> soundEffects_;
        std::vector<MIX_Track *> loopingSfxTracks_;
    };

} // namespace SpaceInvaders
