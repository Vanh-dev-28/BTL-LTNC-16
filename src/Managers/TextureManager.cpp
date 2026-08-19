#include "Managers/TextureManager.h"

#include <SDL3_image/SDL_image.h>
#include <iostream>

namespace SpaceInvaders 
{
    TextureManager& TextureManager::instance()
    {
        static TextureManager manager;
        return manager;
    }
    TextureManager::~TextureManager()
    {
        clear();
    }
    SDL_Texture* TextureManager::getTexture(const std::string& id) const
    {
        const auto found = textures_.find(id);
        return found == textures_.end() ? nullptr : found->second;
    }

    bool TextureManager::loadTexture(const std::string& id, const std::string& path, SDL_Renderer* renderer)
    {
        if (renderer == nullptr) 
        {
            return false;
        }
        unloadTexture(id);
        SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
        if (texture == nullptr) 
        {
            return false;
        }
        textures_.emplace(id, texture);
        return true;
    }

    void TextureManager::unloadTexture(const std::string& id)
    {
        const auto found = textures_.find(id);
        if (found != textures_.end()) 
        {
            SDL_DestroyTexture(found->second);
            textures_.erase(found);
        }
    }
    void TextureManager::clear()
    {
        for (const auto& [id, texture] : textures_) 
        {
            (void)id;
            SDL_DestroyTexture(texture);
        }
        textures_.clear();
    }
}
