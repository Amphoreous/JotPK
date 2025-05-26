#pragma once
#include "Screen.hpp"

class HowToPlayScreen : public Screen {
public:
    HowToPlayScreen(AssetManager& assets, const Vector2& pixelScale);
    void Update(float deltaTime) override;
    void Draw() override;

private:
    int m_currentPage;
    static constexpr int MAX_PAGES = 3;
};
