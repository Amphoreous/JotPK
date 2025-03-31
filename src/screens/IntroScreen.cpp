#include "screens/IntroScreen.hpp"

IntroScreen::IntroScreen(AssetManager& assets, const Vector2& pixelScale)
    : Screen(assets, pixelScale), 
    m_phase(IntroPhase::CompanyLogo),
    m_alpha(0),
    m_timer(0),
    m_showPressStart(true),
    m_blinkTimer(0),
    m_gameLogo({0, 0, 0, 0}),
    m_gameLogoDest({0, 0, 0, 0}),
    m_pressStart({0, 0, 0, 0}),
    m_pressStartDest({0, 0, 0, 0}) {}

void IntroScreen::Update(float deltaTime) {
    if (m_phase == IntroPhase::WaitForInput) {
        // Cache logo rectangles on first frame
        if (m_gameLogo.width == 0) {
            m_gameLogo = { 0, 96, 96, 56 };
            float logoScale = 500.0f / 56.0f;
            m_gameLogoDest = {
                GetScreenWidth()/2.0f - (96 * logoScale)/2,
                GetScreenHeight()/2.0f - (56 * logoScale)/2,
                96 * logoScale,
                56 * logoScale
            };
            
            m_pressStart = { 24, 155, 49, 7 };
            float pressStartScale = 250.0f / 49.0f;
            m_pressStartDest = {
                GetScreenWidth()/2.0f - (49 * pressStartScale)/2,
                GetScreenHeight()/2.0f + (56 * logoScale)/2 + 20,
                49 * pressStartScale,
                7 * pressStartScale
            };
        }

        m_blinkTimer += deltaTime;
        if (m_blinkTimer >= BLINK_INTERVAL) {
            m_blinkTimer = 0;
            m_showPressStart = !m_showPressStart;
        }
        
        if (IsKeyPressed(KEY_SPACE)) {
            PlaySound(m_assets.GetSound("pickup_coin")); // Reproducir sonido antes de la animación
            DoBlinkAnimation();
            m_phase = IntroPhase::Done;
            m_isFinished = true;
        }
        return;
    }

    UpdateFade(deltaTime);
}

void IntroScreen::UpdateFade(float deltaTime) {
    m_timer += deltaTime;
    
    if (m_timer < FADE_IN_TIME) {
        m_alpha = m_timer / FADE_IN_TIME;
    }
    else if (m_timer < FADE_IN_TIME + HOLD_TIME) {
        m_alpha = 1.0f;
    }
    else if (m_timer < FADE_IN_TIME + HOLD_TIME + FADE_OUT_TIME) {
        m_alpha = 1.0f - (m_timer - (FADE_IN_TIME + HOLD_TIME)) / FADE_OUT_TIME;
    }
    else {
        m_timer = 0;
        m_alpha = 0;
        
        switch (m_phase) {
            case IntroPhase::CompanyLogo:
                m_phase = IntroPhase::TeamMembers;
                break;
            case IntroPhase::TeamMembers:
                m_phase = IntroPhase::ProjectInfo;
                break;
            case IntroPhase::ProjectInfo:
                m_phase = IntroPhase::Description;
                break;
            case IntroPhase::Description:
                m_phase = IntroPhase::WaitForInput;
                break;
            default:
                break;
        }
    }
}

void IntroScreen::Draw() {
    if (m_phase == IntroPhase::WaitForInput) {
        float centerX = GetScreenWidth() / 2.0f;
        float centerY = GetScreenHeight() / 2.0f;
        float screenHeight = GetScreenHeight();

        // Game logo un poco más pequeño
        Rectangle logoSource = { 0, 96, 96, 56 };
        float logoScale = 400.0f / 56.0f;  // Reducido de 500 a 400
        Rectangle logoDest = {
            centerX - (96 * logoScale)/2,
            centerY - (56 * logoScale)/2 - 50,
            96 * logoScale,
            56 * logoScale
        };
        
        DrawTexturePro(m_assets.GetTexture("cursors"), logoSource, logoDest,
                      Vector2{0, 0}, 0.0f, WHITE);

        if (m_showPressStart) {
            Rectangle pressStartSource = { 24, 155, 49, 7 };
            float pressStartScale = 300.0f / 49.0f;  // Aumentado de 250 a 300
            Rectangle pressStartDest = {
                centerX - (49 * pressStartScale)/2,
                centerY + (56 * logoScale)/2 + 30,  // Ajustado el espaciado
                49 * pressStartScale,
                7 * pressStartScale
            };
            DrawTexturePro(m_assets.GetTexture("cursors"), pressStartSource, pressStartDest,
                          Vector2{0, 0}, 0.0f, WHITE);
        }

        return;
    }

    float centerX = GetScreenWidth() / 2.0f;
    float centerY = GetScreenHeight() / 2.0f;

    switch (m_phase) {
        case IntroPhase::CompanyLogo: {
            // Aplicar un fade-in adicional al logo
            float logoFade = m_alpha;
            if (m_timer < FADE_IN_TIME * 0.5f) {
                // Primer 50% de tiempo: fade-in solo para el logo, no para el texto
                logoFade = m_timer / (FADE_IN_TIME * 0.5f);
            }
            
            // Logo primero
            Rectangle logoSource = { 304, 160, 16, 16 };
            float logoScale = 512.0f / 16.0f;
            Rectangle logoDest = {
                centerX - (16 * logoScale)/2,
                centerY - (16 * logoScale)/2 - 40,  // Subimos el logo un poco
                16 * logoScale,
                16 * logoScale
            };
            
            DrawTexturePro(m_assets.GetTexture("cursors"), logoSource, logoDest,
                          Vector2{0, 0}, 0.0f, ColorAlpha(WHITE, logoFade));

            // Título de la compañía - con fade-in normal
            const char* companyName = "Amphoreous";
            float fontSize = 80.0f;
            
            Font titleFont = m_assets.GetFont("title");
            Vector2 textSize = MeasureTextEx(titleFont, companyName, fontSize, 2);
            
            // Calculamos la posición vertical relativa al logo
            float textY = logoDest.y + logoDest.height + 40;  // 40 píxeles debajo del logo
            
            // Dibujamos el texto con sombra para mejor legibilidad
            Color shadowColor = ColorAlpha(BLACK, m_alpha * 0.5f);
            Color textColor = ColorAlpha(WHITE, m_alpha);
            
            // Sombra
            DrawTextEx(titleFont, companyName,
                      Vector2{centerX - textSize.x/2 + 2,
                             textY + 2},
                      fontSize, 2, shadowColor);
            
            // Texto principal
            DrawTextEx(titleFont, companyName,
                      Vector2{centerX - textSize.x/2,
                             textY},
                      fontSize, 2, textColor);
            break;
        }
        
        case IntroPhase::TeamMembers: {
            const char* text = 
                "Made By\n\n"
                "Zakaria Hamdaoui\n"
                "Sofia Giner Vargas\n"
                "Joel Martinez Arjona";
                
            Vector2 textSize = MeasureTextEx(m_assets.GetFont("text"), text, 40, 2);
            DrawTextEx(m_assets.GetFont("text"), text,
                      Vector2{centerX - textSize.x/2,
                             centerY - textSize.y/2},
                      40, 2, ColorAlpha(WHITE, m_alpha));
            break;
        }
        
        case IntroPhase::ProjectInfo: {
            Font titleFont = m_assets.GetFont("title");
            Font textFont = m_assets.GetFont("text");
            
            const char* titleText = "Project I";
            Vector2 titleSize = MeasureTextEx(titleFont, titleText, 40, 2);
            DrawTextEx(titleFont, titleText,
                      Vector2{centerX - titleSize.x/2,
                             centerY - 100},
                      40, 2, ColorAlpha(WHITE, m_alpha));
            
            const char* subtextA = "Design and Development of Videogames";
            const char* subtextB = "CITM - UPC";
            const char* subtextC = "Tutor: Alejandro Paris Gomez";
            
            Vector2 sizeA = MeasureTextEx(textFont, subtextA, 40, 2);
            Vector2 sizeB = MeasureTextEx(textFont, subtextB, 40, 2);
            Vector2 sizeC = MeasureTextEx(textFont, subtextC, 40, 2);
            
            DrawTextEx(textFont, subtextA,
                      Vector2{centerX - sizeA.x/2, centerY - 20},
                      40, 2, ColorAlpha(WHITE, m_alpha));
            DrawTextEx(textFont, subtextB,
                      Vector2{centerX - sizeB.x/2, centerY + 20},
                      40, 2, ColorAlpha(WHITE, m_alpha));
            DrawTextEx(textFont, subtextC,
                      Vector2{centerX - sizeC.x/2, centerY + 80},
                      40, 2, ColorAlpha(WHITE, m_alpha));
            break;
        }
        
        case IntroPhase::Description: {
            Font titleFont = m_assets.GetFont("title");
            Font textFont = m_assets.GetFont("text");
            
            const char* titleText = "Journey of the Prairie King";
            Vector2 titleSize = MeasureTextEx(titleFont, titleText, 40, 2);
            DrawTextEx(titleFont, titleText,
                      Vector2{centerX - titleSize.x/2,
                             centerY - 100},
                      40, 2, ColorAlpha(WHITE, m_alpha));
            
            const char* subtextA = "Play as the Prairie King, fight against several adversaries,";
            const char* subtextB = "upgrade your equipment, use some powerful power-ups to collect";
            const char* subtextC = "a few items that will help you get to your fiancee";
            
            Vector2 sizeA = MeasureTextEx(textFont, subtextA, 40, 2);
            Vector2 sizeB = MeasureTextEx(textFont, subtextB, 40, 2);
            Vector2 sizeC = MeasureTextEx(textFont, subtextC, 40, 2);
            
            DrawTextEx(textFont, subtextA,
                      Vector2{centerX - sizeA.x/2, centerY - 20},
                      40, 2, ColorAlpha(WHITE, m_alpha));
            DrawTextEx(textFont, subtextB,
                      Vector2{centerX - sizeB.x/2, centerY + 20},
                      40, 2, ColorAlpha(WHITE, m_alpha));
            DrawTextEx(textFont, subtextC,
                      Vector2{centerX - sizeC.x/2, centerY + 60},
                      40, 2, ColorAlpha(WHITE, m_alpha));
            break;
        }
        
        default:
            break;
    }
}

void IntroScreen::DoBlinkAnimation() {
    // Actualizar las mismas dimensiones aquí también
    float centerX = GetScreenWidth() / 2.0f;
    float centerY = GetScreenHeight() / 2.0f;
    float screenHeight = GetScreenHeight();

    Rectangle logoSource = { 0, 96, 96, 56 };
    float logoScale = 400.0f / 56.0f;
    Rectangle logoDest = {
        centerX - (96 * logoScale)/2,
        centerY - (56 * logoScale)/2 - 50,
        96 * logoScale,
        56 * logoScale
    };

    Rectangle pressStartSource = { 24, 155, 49, 7 };
    float pressStartScale = 300.0f / 49.0f;
    Rectangle pressStartDest = {
        centerX - (49 * pressStartScale)/2,
        centerY + (56 * logoScale)/2 + 30,
        49 * pressStartScale,
        7 * pressStartScale
    };

    // Quick blink animation
    for (int i = 0; i < 10; i++) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(m_assets.GetTexture("cursors"), logoSource, logoDest,
                      Vector2{0, 0}, 0.0f, WHITE);

        if (i % 2 == 0) {
            DrawTexturePro(m_assets.GetTexture("cursors"), pressStartSource, pressStartDest,
                          Vector2{0, 0}, 0.0f, WHITE);
        }

        EndDrawing();
        WaitTime(0.1f);
    }
    
    // Final pause with just the logo
    for (int i = 0; i < 15; i++) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(m_assets.GetTexture("cursors"), logoSource, logoDest,
                      Vector2{0, 0}, 0.0f, WHITE);

        EndDrawing();
        WaitTime(0.1f);
    }
}
