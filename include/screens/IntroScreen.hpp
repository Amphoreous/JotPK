#pragma once
#include "Screen.hpp"

class IntroScreen : public Screen {
public:
    IntroScreen(AssetManager& assets, const Vector2& pixelScale);
    void Update(float deltaTime) override;
    void Draw() override;

private:
    enum class IntroPhase {
        CompanyLogo,
        TeamMembers,
        ProjectInfo,
        Description,
        WaitForInput,
        Done
    };

    void UpdateFade(float deltaTime);
    void DrawPhase();
    
    IntroPhase m_phase;
    float m_alpha;
    float m_timer;
    bool m_showPressStart;
    float m_blinkTimer;

    static constexpr float FADE_IN_TIME = 1.0f;
    static constexpr float HOLD_TIME = 2.0f;
    static constexpr float FADE_OUT_TIME = 1.0f;
    static constexpr float BLINK_INTERVAL = 0.5f;

    // Add new members for logo positions
    Rectangle m_gameLogo;
    Rectangle m_gameLogoDest;
    Rectangle m_pressStart;
    Rectangle m_pressStartDest;
    
    void DoBlinkAnimation();
};
