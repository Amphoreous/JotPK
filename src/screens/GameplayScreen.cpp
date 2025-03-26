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

    m_game->Update(deltaTime);
}

void GameplayScreen::Draw() {
    m_game->Draw();
}

bool GameplayScreen::IsFinished() const {
    return m_game->IsGameOver();
}
