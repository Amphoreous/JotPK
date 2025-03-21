#include "screens/GameplayScreen.hpp"

GameplayScreen::GameplayScreen(AssetManager& assets, const Vector2& pixelScale)
    : Screen(assets, pixelScale)
{
    m_game = std::make_unique<PrairieKing>(assets);
    
    // Set music as in original
    PlayMusicStream(assets.GetMusic("overworld"));
    SetMusicVolume(assets.GetMusic("overworld"), 0.7f);
}

void GameplayScreen::Update(float deltaTime) {
    UpdateMusicStream(m_assets.GetMusic("overworld"));
    m_game->Update(deltaTime);
}

void GameplayScreen::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    m_game->Draw();
    EndDrawing();
}

bool GameplayScreen::IsFinished() const {
    return m_game->IsGameOver();
}
