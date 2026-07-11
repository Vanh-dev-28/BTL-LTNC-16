#pragma once

#include <SDL.h>
#include <string>
#include <unordered_map>

namespace SpaceInvaders {

/** Caches SDL textures and centralizes their ownership. */
class TextureManager {
public:
    static TextureManager& instance();

    bool loadTexture(const std::string& id, const std::string& path, SDL_Renderer* renderer);
    [[nodiscard]] SDL_Texture* getTexture(const std::string& id) const;
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
