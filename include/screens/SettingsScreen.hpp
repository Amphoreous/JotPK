#pragma once
#include "Screen.hpp"

enum class DisplayMode {
    FULLSCREEN,
    BORDERLESS,
    WINDOWED
};

class SettingsScreen : public Screen {
public:
    SettingsScreen(AssetManager& assets, const Vector2& pixelScale);
    void Update(float deltaTime) override;
    void Draw() override;

private:
    void ToggleFullscreenMode();
    DisplayMode GetDisplayMode();
    void CycleDisplayMode();

    DisplayMode m_displayMode;
    int m_selectedOption;
    int m_volume;
    float m_blinkTimer;
    bool m_showPrompt;
    static constexpr float BLINK_INTERVAL = 0.5f;
};
