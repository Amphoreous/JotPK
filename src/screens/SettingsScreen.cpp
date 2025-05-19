#include "screens/SettingsScreen.hpp"

SettingsScreen::SettingsScreen(AssetManager& assets, const Vector2& pixelScale)
    : Screen(assets, pixelScale),
    m_isFullscreen(false),
    m_selectedOption(0),
    m_volume(100),
    m_blinkTimer(0),
    m_showPrompt(true) {}

void SettingsScreen::Update(float deltaTime) {
    m_blinkTimer += deltaTime;
    if (m_blinkTimer >= BLINK_INTERVAL) {
        m_blinkTimer = 0;
        m_showPrompt = !m_showPrompt;
    }

    if (IsKeyPressed(KEY_UP)) m_selectedOption = (m_selectedOption - 1 + 3) % 3;
    if (IsKeyPressed(KEY_DOWN)) m_selectedOption = (m_selectedOption + 1) % 3;

    if (IsKeyPressed(KEY_ENTER)) {
        switch (m_selectedOption) {
            case 0: // Volume
                break;
            case 1: // Fullscreen
                m_isFullscreen = !m_isFullscreen;
                ToggleFullscreen();
                break;
            case 2: // Back
                m_isFinished = true;
                break;
        }
    }

    if (m_selectedOption == 0) {
        if (IsKeyPressed(KEY_LEFT)) m_volume = std::max(0, m_volume - 10);
        if (IsKeyPressed(KEY_RIGHT)) m_volume = std::min(100, m_volume + 10);
        SetMasterVolume(m_volume / 100.0f);
    }

    if (m_selectedOption == 1)
    {
        if (IsKeyPressed(KEY_ENTER)) ToggleFullscreen;

    }

    if (IsKeyPressed(KEY_ESCAPE)) m_isFinished = true;
}

void SettingsScreen::Draw() {
    float centerX = GetScreenWidth() / 2.0f;
    float centerY = GetScreenHeight() / 2.0f;

    DrawTextEx(m_assets.GetFont("title"), "Settings",
               Vector2{centerX - 100, centerY - 150}, 40, 2, WHITE);

    const char* options[] = {
        TextFormat("Volume: %d%%", m_volume),
        TextFormat("Fullscreen: %s", m_isFullscreen ? "Off" : "On"),
        "Back"
    };

    for (int i = 0; i < 3; i++) {
        Color color = (i == m_selectedOption) ? RED : WHITE;
        DrawTextEx(m_assets.GetFont("text"), options[i],
                  Vector2{centerX - 100, centerY - 50 + i * 40}, 30, 2, color);
    }
}
