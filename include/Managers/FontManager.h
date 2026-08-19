#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <unordered_map>

namespace SpaceInvaders 
{
    class FontManager 
    {
    public:
        static FontManager& instance();
        bool loadFont(const std::string& id, const std::string& path, int pointSize);
        [[nodiscard]] TTF_Font* getFont(const std::string& id) const;
        void clear();
    private:
        FontManager() = default;
        ~FontManager();
        FontManager(const FontManager&) = delete;
        FontManager& operator=(const FontManager&) = delete;
        std::unordered_map<std::string, TTF_Font*> fonts_;
    };
} // namespace SpaceInvades