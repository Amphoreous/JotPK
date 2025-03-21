#pragma once
#include "Screen.hpp"
#include "../gameplay/PrairieKing.hpp"

class GameplayScreen : public Screen {
public:
    GameplayScreen(AssetManager& assets, const Vector2& pixelScale);
    
    virtual void Update(float deltaTime) override;
    virtual void Draw() override;
    virtual bool IsFinished() const override;

private:
    std::unique_ptr<PrairieKing> m_game;
};
