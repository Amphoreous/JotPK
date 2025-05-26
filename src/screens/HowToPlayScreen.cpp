#include "screens/HowToPlayScreen.hpp"

HowToPlayScreen::HowToPlayScreen(AssetManager& assets, const Vector2& pixelScale)
    : Screen(assets, pixelScale), 
    m_currentPage(0) {}

void HowToPlayScreen::Update(float deltaTime) {
    if (IsKeyPressed(KEY_RIGHT) && m_currentPage < MAX_PAGES - 1) m_currentPage++;
    if (IsKeyPressed(KEY_LEFT) && m_currentPage > 0) m_currentPage--;
    if (IsKeyPressed(KEY_ESCAPE)) m_isFinished = true;
}

void HowToPlayScreen::Draw() {
    float centerX = GetScreenWidth() / 2.0f;
    float centerY = GetScreenHeight() / 2.0f;

    DrawTextEx(m_assets.GetFont("title"), "How To Play",
               Vector2{centerX - 100, 50}, 40, 2, WHITE);

    // Draw different instructions based on current page
    switch (m_currentPage) {
        case 0:
            DrawTextEx(m_assets.GetFont("text"), "Use WASD to move",
                      Vector2{centerX - 200, centerY - 50}, 30, 2, WHITE);
            DrawTextEx(m_assets.GetFont("text"), "Press Arrow Keys to shoot",
                      Vector2{centerX - 200, centerY}, 30, 2, WHITE);
            break;
        case 1:
            DrawTextEx(m_assets.GetFont("text"), "Space to use power-ups",
                      Vector2{centerX - 200, centerY - 50}, 30, 2, WHITE);
            DrawTextEx(m_assets.GetFont("text"), "Kill enemies and get upgrades",
                      Vector2{centerX - 200, centerY}, 30, 2, WHITE);
            break;
        case 2:
            DrawTextEx(m_assets.GetFont("text"), "Reach the end of each level",
                      Vector2{centerX - 200, centerY - 50}, 30, 2, WHITE);
            DrawTextEx(m_assets.GetFont("text"), "to progress through the game",
                      Vector2{centerX - 200, centerY}, 30, 2, WHITE);
            break;
    }

    // Draw page indicators
    DrawTextEx(m_assets.GetFont("text"), 
               TextFormat("Page %d/%d", m_currentPage + 1, MAX_PAGES),
               Vector2{centerX - 50.0f, GetScreenHeight() - 100.0f}, 
               20.0f, 2.0f, WHITE);
               
    // Draw Back button
    const char* backText = "Back";
    Vector2 textSize = MeasureTextEx(m_assets.GetFont("text"), backText, 30, 2);
    float backY = GetScreenHeight() - 80.0f;
    DrawTextEx(m_assets.GetFont("text"), backText,
               Vector2{centerX - textSize.x/2, backY},
               30, 2, WHITE);

    // Añadir interacción con el botón Back
    Vector2 mousePos = GetMousePosition();
    Rectangle backButton = {
        centerX - textSize.x/2,
        backY,
        textSize.x,
        textSize.y
    };

    if (CheckCollisionPointRec(mousePos, backButton)) {
        DrawTextEx(m_assets.GetFont("text"), backText,
                   Vector2{centerX - textSize.x/2, backY},
                   30, 2, RED);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            m_isFinished = true;
        }
    }

    // Draw navigation help
    DrawTextEx(m_assets.GetFont("text"), "Use Left/Right arrows to navigate pages",
               Vector2{centerX - 200.0f, GetScreenHeight() - 40.0f},
               20, 2, GRAY);
}
