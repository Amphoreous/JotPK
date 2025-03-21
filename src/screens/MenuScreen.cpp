#include "screens/MenuScreen.hpp"

MenuScreen::MenuScreen(AssetManager& assets, const Vector2& pixelScale)
    : Screen(assets, pixelScale), m_selectedOption(-1), m_blinkTimer(0), m_showPrompt(true) {}

void MenuScreen::Update(float deltaTime) {
    m_blinkTimer += deltaTime;
    if (m_blinkTimer >= BLINK_INTERVAL) {
        m_blinkTimer = 0;
        m_showPrompt = !m_showPrompt;
    }

    Vector2 mousePos = GetMousePosition();
    
    // Check mouse hover over options
    const char* options[] = { "Start", "Settings", "How to Play", "Exit" };
    for (int i = 0; i < 4; i++) {
        MenuScreen::MenuOption bounds = GetOptionBounds(options[i], i);
        // Convert MenuOption to Rectangle for collision check
        Rectangle hitbox = { bounds.x, bounds.y, bounds.width, bounds.height };
        if (CheckCollisionPointRec(mousePos, hitbox)) {
            m_selectedOption = i;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (i == 3) CloseWindow();
                else m_isFinished = true;
            }
            break;
        }
    }

    // Keyboard navigation
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) 
        m_selectedOption = (m_selectedOption - 1 + 4) % 4;
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
        m_selectedOption = (m_selectedOption + 1) % 4;
        if (IsKeyPressed(KEY_ENTER) && m_selectedOption >= 0) {
        if (m_selectedOption == 3) CloseWindow();
        else m_isFinished = true;
    }
}

void MenuScreen::Draw() {
    float centerX = GetScreenWidth() / 2.0f;
    float centerY = GetScreenHeight() / 2.0f;
    float screenHeight = GetScreenHeight();
    float screenWidth = GetScreenWidth();

    // Game logo ajustado para coincidir con IntroScreen
    const Rectangle logoSource = { 128, 1744, 96, 56 };
    const float logoScale = 400.0f / 56.0f;  // Mismo scale que IntroScreen
    const Rectangle logoDest = {
        centerX - (96 * logoScale)/2,
        centerY - (56 * logoScale)/2 - 50,  // Ajustado para coincidir con IntroScreen
        96 * logoScale,
        56 * logoScale
    };
    
    DrawTexturePro(m_assets.GetTexture("cursors"), 
                   logoSource, logoDest,
                   Vector2{ 0, 0 }, 0.0f, WHITE);

    // Menu options ajustados a la nueva posición del logo
    const char* options[] = { "Start", "How to Play", "Settings", "Exit" };
    const float menuY = centerY + (56 * logoScale)/2 + 30;  // Mismo espaciado que IntroScreen
    const float totalMenuWidth = screenWidth * 0.6f;
    const float itemSpacing = totalMenuWidth / 4.0f;
    const float startX = (screenWidth - totalMenuWidth) / 2.0f;

    const float textSpacing = 8.0f;  // Espaciado consistente
    // Menu options con espaciado ajustado y tamaño más grande
    for (int i = 0; i < 4; i++) {
        MenuOption bounds = GetOptionBounds(options[i], i);
        Color color = (i == m_selectedOption) ? RED : WHITE;
        DrawTextEx(m_assets.GetFont("text"), options[i], 
                  Vector2{bounds.x, bounds.y}, 
                  40, 2, color);  // Aumentado de 20 a 30 el tamaño
    }

    // Footer con copyright y botones - ajustado al borde inferior
    const float footerY = screenHeight - 40;

    // Copyright en el lado izquierdo
    DrawTextEx(m_assets.GetFont("text"),
              TextFormat("(c) Copyright. Amphoreous 2025. All rights reserved."),
              Vector2{20, footerY},
              20, 2, WHITE);

    // Botones de web en el lado derecho
    const Rectangle githubSource = { 320, 1648, 32, 32 };  // Nueva ubicación del logo de GitHub
    const Rectangle webSource = { 832, 1808, 16, 16 };
    const float buttonSize = 32; // 16 * 2.0f scale
    const float buttonSpacing = 10.0f;
    const float rightMargin = 20.0f;

    // Botón de GitHub
    const Rectangle githubDest = {
        screenWidth - (buttonSize * 2) - buttonSpacing - rightMargin,
        footerY - 5,
        buttonSize,
        buttonSize
    };
    
    // Botón de Web
    const Rectangle webDest = {
        screenWidth - buttonSize - rightMargin,
        footerY - 5,
        buttonSize,
        buttonSize
    };

    DrawTexturePro(m_assets.GetTexture("cursors"), githubSource, githubDest,
                   Vector2{ 0, 0 }, 0.0f, WHITE);
    DrawTexturePro(m_assets.GetTexture("cursors"), webSource, webDest,
                   Vector2{ 0, 0 }, 0.0f, WHITE);

    // Manejo de clicks
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePos, githubDest)) {
            OpenURL("https://github.com/Amphoreous/JotPK");
        }
        else if (CheckCollisionPointRec(mousePos, webDest)) {
            OpenURL("https://amphoreous.github.io/JotPK/");
        }
    }
}

MenuScreen::MenuOption MenuScreen::GetOptionBounds(const char* text, int index) const {
    float screenWidth = GetScreenWidth();
    float centerY = GetScreenHeight() / 2.0f;
    const float logoScale = 400.0f / 56.0f;  // Mantener coherencia con Draw()
    const float menuY = centerY + (56 * logoScale)/2 + 30;  // Mismo espaciado que en Draw()
    
    const float totalMenuWidth = screenWidth * 0.6f;
    const float itemSpacing = totalMenuWidth / 4.0f;
    const float startX = (screenWidth - totalMenuWidth) / 2.0f;
    
    float textWidth = MeasureTextEx(m_assets.GetFont("text"), text, 40, 2).x;  // Aumentado de 20 a 30 para coincidir con Draw()
    return MenuScreen::MenuOption{
        startX + (itemSpacing * index) + (itemSpacing / 2.0f) - (textWidth / 2.0f),
        menuY,
        textWidth,
        40.0f  // Aumentado de 20 a 30 para coincidir con Draw()
    };
}
