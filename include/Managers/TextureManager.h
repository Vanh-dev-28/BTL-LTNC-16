#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>

namespace SpaceInvaders 
{
    class TextureManager 
    {
    public:
        [[nodiscard]] SDL_Texture* getTexture(const std::string& id) const;
        static TextureManager& instance();
        bool loadTexture(const std::string& id, const std::string& path, SDL_Renderer* renderer);
        void unloadTexture(const std::string& id);
        void clear();
    private:
        TextureManager() = default;
        ~TextureManager();
        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;
        std::unordered_map<std::string, SDL_Texture*> textures_;
    };
} // namespace SpaceInvaders
