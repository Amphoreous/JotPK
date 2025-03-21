#pragma once
#include "Screen.hpp"

class SettingsScreen : public Screen {
public:
    SettingsScreen(AssetManager& assets, const Vector2& pixelScale);
    void Update(float deltaTime) override;
    void Draw() override;

private:
    bool m_isFullscreen;
    int m_selectedOption;
    int m_volume;
    float m_blinkTimer;
    bool m_showPrompt;
    static constexpr float BLINK_INTERVAL = 0.5f;
};
