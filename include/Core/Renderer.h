#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

namespace SpaceInvaders
{

    class Renderer
    {
    public:
        static float s_scale;
        static float s_offsetX;
        static float s_offsetY;
        Renderer() = default;
        ~Renderer();
        [[nodiscard]] SDL_Renderer *getSDLRenderer() const;
        bool create(SDL_Window *window);
        void clear();
        void drawText(const std::string &text, TTF_Font *font, SDL_Color color, int x, int y);
        void present();
        void drawTexture(SDL_Texture *texture, float x, float y, float width, float height, const SDL_FRect *sourceRect = nullptr);
        void destroy();
        bool measureText(const std::string &text, TTF_Font *font, int &width, int &height) const;
        void drawTextCentered(const std::string &text, TTF_Font *font, SDL_Color color, int centerX,int y); 
        void drawRect(float x, float y, float width, float height, SDL_Color color);
        void fillRect(float x, float y, float width, float height, SDL_Color color);
        void drawLine(float x1, float y1, float x2, float y2, SDL_Color color);
        void updateViewport(SDL_Window *window);
    private:
        SDL_Renderer *renderer_{nullptr};
        float scale_{1.0f};
        float offsetX_{0.0f};
        float offsetY_{0.0f};
    };

} // namespace SpaceInvaders