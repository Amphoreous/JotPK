#pragma once
#include "Screen.hpp"

class MenuScreen : public Screen {
public:
    MenuScreen(AssetManager& assets, const Vector2& pixelScale);
    void Update(float deltaTime) override;
    void Draw() override;
    int GetSelectedOption() const { return m_selectedOption; }

private:
    int m_selectedOption;
    float m_blinkTimer;
    bool m_showPrompt;
    const float BLINK_INTERVAL = 0.5f;
    struct MenuOption {
        float x;
        float y;
        float width;
        float height;
        
        // Add conversion operator to Rectangle for easier use with raylib functions
        operator Rectangle() const { return Rectangle{ x, y, width, height }; }
    };
    MenuOption GetOptionBounds(const char* text, int index) const;
};
