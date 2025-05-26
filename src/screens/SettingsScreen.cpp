#include "screens/SettingsScreen.hpp"

SettingsScreen::SettingsScreen(AssetManager& assets, const Vector2& pixelScale)
    : Screen(assets, pixelScale),
    m_displayMode(GetDisplayMode()),
    m_selectedOption(0),
    m_volume(GetMasterVolume() * 100),
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
            case 1: // Display Mode
                CycleDisplayMode();
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

    if (IsKeyPressed(KEY_ESCAPE)) m_isFinished = true;
}

void SettingsScreen::Draw() {
    float centerX = GetScreenWidth() / 2.0f;
    float centerY = GetScreenHeight() / 2.0f;

    DrawTextEx(m_assets.GetFont("title"), "Settings",
               Vector2{centerX - 100, centerY - 150}, 40, 2, WHITE);

    const char* displayModeText;
    switch (m_displayMode) {
        case DisplayMode::FULLSCREEN:
            displayModeText = "Fullscreen";
            break;
        case DisplayMode::BORDERLESS:
            displayModeText = "Borderless Windowed";
            break;
        case DisplayMode::WINDOWED:
            displayModeText = "Windowed";
            break;
    }

    const char* options[] = {
        TextFormat("Volume: %d%%", m_volume),
        TextFormat("Display Mode: %s", displayModeText),
        "Back"
    };

    for (int i = 0; i < 3; i++) {
        Color color = (i == m_selectedOption) ? RED : WHITE;
        DrawTextEx(m_assets.GetFont("text"), options[i],
                  Vector2{centerX - 100, centerY - 50 + i * 40}, 30, 2, color);
    }
}

DisplayMode SettingsScreen::GetDisplayMode() {
    if (IsWindowFullscreen()) {
        return DisplayMode::FULLSCREEN;
    }
    
    // Check if it's borderless by comparing window size to monitor size
    int display = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(display);
    int monitorHeight = GetMonitorHeight(display);
    
    if (GetScreenWidth() == monitorWidth && GetScreenHeight() == monitorHeight) {
        return DisplayMode::BORDERLESS;
    }
    
    return DisplayMode::WINDOWED;
}

void SettingsScreen::CycleDisplayMode() {
    int display = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(display);
    int monitorHeight = GetMonitorHeight(display);
    
    switch (m_displayMode) {
        case DisplayMode::FULLSCREEN:
            // Switch to borderless windowed
            if (IsWindowFullscreen()) {
                ToggleFullscreen();
            }
            SetWindowSize(monitorWidth, monitorHeight);
            SetWindowPosition(0, 0); // Position at top-left for borderless
            m_displayMode = DisplayMode::BORDERLESS;
            break;
            
        case DisplayMode::BORDERLESS:
        {
            // Switch to windowed (added braces to create a new scope)
            int windowWidth = (int)(monitorWidth * 0.8f);
            int windowHeight = (int)(monitorHeight * 0.8f);
            
            // Calculate center position
            int centerPosX = (monitorWidth - windowWidth) / 2;
            int centerPosY = (monitorHeight - windowHeight) / 2;
            
            SetWindowSize(windowWidth, windowHeight);
            SetWindowPosition(centerPosX, centerPosY); // Center the window
            
            m_displayMode = DisplayMode::WINDOWED;
            break;
        }
            
        case DisplayMode::WINDOWED:
            // Switch to fullscreen
            SetWindowSize(monitorWidth, monitorHeight);
            SetWindowPosition(0, 0); // Position at top-left before fullscreen
            if (!IsWindowFullscreen()) {
                ToggleFullscreen();
            }
            m_displayMode = DisplayMode::FULLSCREEN;
            break;
    }
}
