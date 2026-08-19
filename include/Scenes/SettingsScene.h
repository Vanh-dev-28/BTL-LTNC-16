#pragma once

#include "Scenes/Scene.h"
#include <vector>
#include <string>

namespace SpaceInvaders
{
    class SettingsScene : public Scene
    {
        public:
            enum class SettingsState
            {
                Normal,
                ResolutionPopup
            };
            SettingsScene() = default;
            ~SettingsScene() override = default;
            void enter() override;
            void exit() override;
            void update(float deltaTime) override;
            void render(Renderer& renderer) override;
            void updateNormal();
            void updateResolutionPopup();
            void renderResolutionPopup(Renderer& renderer);
        private:
            std::vector<std::string> settingItems_
            {
                "Resolution",
                "Music Volume",
                "Apply",
                "Discard"
            };
            std::vector<std::string> resolutionOptions_
            {
                "1280x720",
                "1600x900",
                "1920x1080"
            };
            void drawVolumeBar(Renderer& renderer, const std::string& label, int volume, int y, bool selected);
            int selectedIndex_ = 0;
            SettingsState state_ = SettingsState::Normal;
            int resolutionPopupIndex_ = 0;
            void updateVolumeSlider(int index, float mouseX, float mouseY);
            bool isMouseOverVolumeSlider(int index, float mouseX, float mouseY) const;
            int draggingSlider_ = -1;
        };
}; // namespace SpaceInvaders