#include "Managers/FontManager.h"

namespace SpaceInvaders {

FontManager& FontManager::instance()
{
    static FontManager manager;
    return manager;
}

FontManager::~FontManager()
{
    clear();
}

bool FontManager::loadFont(const std::string& id, const std::string& path, int pointSize)
{
    const auto existing = fonts_.find(id);
    if (existing != fonts_.end()) {
        TTF_CloseFont(existing->second);
        fonts_.erase(existing);
    }
    TTF_Font* font = TTF_OpenFont(path.c_str(), pointSize);
    if (font == nullptr) {
        return false;
    }
    fonts_.emplace(id, font);
    return true;
}

TTF_Font* FontManager::getFont(const std::string& id) const
{
    const auto found = fonts_.find(id);
    return found == fonts_.end() ? nullptr : found->second;
}

void FontManager::clear()
{
    for (const auto& [id, font] : fonts_) {
        (void)id;
        TTF_CloseFont(font);
    }
    fonts_.clear();
}

} // namespace SpaceInvaders
