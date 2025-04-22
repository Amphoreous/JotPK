#include "screens/GameplayScreen.hpp"
#include "gameplay/PrairieKing.hpp"

GameplayScreen::GameplayScreen(AssetManager& assets, const Vector2& pixelScale)
    : Screen(assets, pixelScale)
{
    m_game = std::make_unique<PrairieKing>(assets);
}

void GameplayScreen::Update(float deltaTime) {

    // Update input states for game
    if (IsKeyDown(KEY_W))
        m_game->SetButtonState(PrairieKing::GameKeys::MoveUp, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::MoveUp, false);

    if (IsKeyDown(KEY_S))
        m_game->SetButtonState(PrairieKing::GameKeys::MoveDown, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::MoveDown, false);

    if (IsKeyDown(KEY_A))
        m_game->SetButtonState(PrairieKing::GameKeys::MoveLeft, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::MoveLeft, false);

    if (IsKeyDown(KEY_D))
        m_game->SetButtonState(PrairieKing::GameKeys::MoveRight, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::MoveRight, false);

    if (IsKeyDown(KEY_UP))
        m_game->SetButtonState(PrairieKing::GameKeys::ShootUp, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::ShootUp, false);

    if (IsKeyDown(KEY_DOWN))
        m_game->SetButtonState(PrairieKing::GameKeys::ShootDown, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::ShootDown, false);

    if (IsKeyDown(KEY_LEFT))
        m_game->SetButtonState(PrairieKing::GameKeys::ShootLeft, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::ShootLeft, false);

    if (IsKeyDown(KEY_RIGHT))
        m_game->SetButtonState(PrairieKing::GameKeys::ShootRight, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::ShootRight, false);

    if (IsKeyDown(KEY_ENTER))
        m_game->SetButtonState(PrairieKing::GameKeys::SelectOption, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::SelectOption, false);

    if (IsKeyDown(KEY_SPACE))
        m_game->SetButtonState(PrairieKing::GameKeys::UsePowerup, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::UsePowerup, false);

    if (IsKeyDown(KEY_ESCAPE))
        m_game->SetButtonState(PrairieKing::GameKeys::Exit, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::Exit, false);

    // Debug toggle (on press only)
    if (IsKeyPressed(KEY_F3))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugToggle, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::DebugToggle, false);

    // Debug actions (allow hold)
    if (IsKeyDown(KEY_F5))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugAddLife, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::DebugAddLife, false);

    if (IsKeyDown(KEY_F6))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugAddCoins, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::DebugAddCoins, false);

    if (IsKeyDown(KEY_F7))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugIncDamage, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::DebugIncDamage, false);

    if (IsKeyDown(KEY_F8))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugClearMonsters, true);
    else
        m_game->SetButtonState(PrairieKing::GameKeys::DebugClearMonsters, false);

    // Monster spawn keys (on press only)
    if (IsKeyPressed(KEY_KP_1))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugSpawn1, true);
    if (IsKeyPressed(KEY_KP_2))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugSpawn2, true);
    if (IsKeyPressed(KEY_KP_3))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugSpawn3, true);
    if (IsKeyPressed(KEY_KP_4))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugSpawn4, true);
    if (IsKeyPressed(KEY_KP_5))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugSpawn5, true);
    if (IsKeyPressed(KEY_KP_6))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugSpawn6, true);
    if (IsKeyPressed(KEY_KP_7))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugSpawn7, true);
    if (IsKeyPressed(KEY_KP_8))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugSpawn8, true);
    if (IsKeyPressed(KEY_KP_9))
        m_game->SetButtonState(PrairieKing::GameKeys::DebugSpawn9, true);

    m_game->Update(deltaTime);
}

void GameplayScreen::Draw() {
    m_game->Draw();
}

bool GameplayScreen::IsFinished() const {
    return m_game->IsGameOver();
}
