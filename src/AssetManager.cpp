#include "AssetManager.hpp"
#include "JsonHelper.hpp"

void AssetManager::LoadAssets() {
    InitAudioDevice();
    
    // Load textures
    m_textures["cursors"] = LoadTexture("cursors/cursors.png");
    
    // Load sounds
    m_sounds["pickup_coin"] = LoadSound("sounds/Pickup_Coin15.wav");
    SetSoundPitch(m_sounds["pickup_coin"], 0.95f);
    m_sounds["cowboy_gunshot"] = LoadSound("sounds/cowboy_gunshot.wav");
    m_sounds["cowboy_dead"] = LoadSound("sounds/cowboy_dead.wav"); 
    m_sounds["cowboy_powerup"] = LoadSound("sounds/cowboy_powerup.wav");
    m_sounds["cowboy_explosion"] = LoadSound("sounds/cowboy_explosion.wav");
    m_sounds["cowboy_footstep"] = LoadSound("sounds/cowboy_footstep.wav");
    m_sounds["cowboy_monsterhit"] = LoadSound("sounds/cowboy_monsterhit.wav");
    m_sounds["cowboy_gopher"] = LoadSound("sounds/cowboy_gopher.wav");
    m_sounds["cowboy_singing"] = LoadSound("sounds/cowboy_singing.wav");
    m_sounds["cowboy_secret"] = LoadSound("sounds/cowboy_secret.wav");
    
    // Load music streams
    LoadMusic("overworld", "music/Cowboy_OVERWORLD.ogg");
    LoadMusic("outlaw", "music/cowboy_outlawsong.ogg"); 
    LoadMusic("zombie", "music/Cowboy_undead.ogg");
    
    // Set default volumes
    for (auto& [name, music] : m_music) {
        SetMusicVolume(music, 0.7f);
    }
    
    // Cargar fuentes
    LoadFonts();
}

void AssetManager::LoadFonts() {
    // Cargar datos de los JSON
    FontData spriteFontData = JsonHelper::LoadFontData("fonts/SpriteFont1/SpriteFont1.json");
    FontData smallFontData = JsonHelper::LoadFontData("fonts/SmallFont/SmallFont.json");

    // Cargar texturas directamente
    Texture2D titleAtlas = LoadTexture("fonts/SpriteFont1/SpriteFont1.png");
    Texture2D textAtlas = LoadTexture("fonts/SmallFont/SmallFont.png");

    // Configurar la fuente de título
    {
        Font titleFont = { 0 };
        titleFont.baseSize = spriteFontData.verticalLineSpacing;
        titleFont.glyphCount = 256;
        titleFont.glyphs = (GlyphInfo *)MemAlloc(titleFont.glyphCount * sizeof(GlyphInfo));
        titleFont.recs = (Rectangle *)MemAlloc(titleFont.glyphCount * sizeof(Rectangle));
        titleFont.texture = titleAtlas;

        // Inicializar todos los glifos con valores por defecto
        for (int i = 0; i < titleFont.glyphCount; i++) {
            titleFont.glyphs[i].value = i;
            titleFont.glyphs[i].offsetX = 0;
            titleFont.glyphs[i].offsetY = 0;
            titleFont.glyphs[i].advanceX = spriteFontData.verticalLineSpacing/2;  // Valor por defecto razonable

            titleFont.recs[i] = Rectangle{0, 0, 0, 0};
        }

        // Configurar los glifos usando el mapa de caracteres
        for (const auto& pair : spriteFontData.characters) {
            char c = pair.first;
            const auto& glyphData = pair.second;
            int i = (int)c;

            if (i >= 0 && i < titleFont.glyphCount) {
                titleFont.glyphs[i].value = i;
                titleFont.glyphs[i].offsetX = glyphData.cropX;
                titleFont.glyphs[i].offsetY = glyphData.cropY;
                // Aumentar el espaciado entre caracteres
                titleFont.glyphs[i].advanceX = glyphData.cropWidth + spriteFontData.horizontalSpacing + 4; // Añadido +6 para más espacio

                titleFont.recs[i].x = (float)glyphData.texX;
                titleFont.recs[i].y = (float)glyphData.texY;
                titleFont.recs[i].width = (float)glyphData.texWidth;
                titleFont.recs[i].height = (float)glyphData.texHeight;
            }
        }

        m_fonts["title"] = titleFont;
    }

    // Configurar la fuente pequeña
    {
        Font textFont = { 0 };
        textFont.baseSize = smallFontData.verticalLineSpacing;
        textFont.glyphCount = 256;  // Aumentamos el número de glifos para asegurar que caben todos los caracteres especiales
        textFont.glyphs = (GlyphInfo *)MemAlloc(textFont.glyphCount * sizeof(GlyphInfo));
        textFont.recs = (Rectangle *)MemAlloc(textFont.glyphCount * sizeof(Rectangle));
        textFont.texture = textAtlas;

        // Inicializar todos los glifos con valores por defecto
        for (int i = 0; i < textFont.glyphCount; i++) {
            textFont.glyphs[i].value = i;
            textFont.glyphs[i].offsetX = 0;
            textFont.glyphs[i].offsetY = 0;
            textFont.glyphs[i].advanceX = smallFontData.verticalLineSpacing/2;
            textFont.recs[i] = Rectangle{0, 0, 0, 0};
        }

        // Configurar los glifos usando el mapa de caracteres
        for (size_t i = 0; i < smallFontData.characterMap.size(); i++) {
            char c = smallFontData.characterMap[i][0];  // El primer carácter de cada string en el mapa
            const auto& glyphData = smallFontData.characters[c];

            if (i < textFont.glyphCount) {
                textFont.glyphs[i].value = (int)c;  // Usar el valor ASCII del carácter
                textFont.glyphs[i].offsetX = glyphData.cropX;
                textFont.glyphs[i].offsetY = glyphData.cropY;
                textFont.glyphs[i].advanceX = glyphData.cropWidth + (smallFontData.horizontalSpacing / 2);

                textFont.recs[i].x = (float)glyphData.texX;
                textFont.recs[i].y = (float)glyphData.texY;
                textFont.recs[i].width = (float)glyphData.texWidth;
                textFont.recs[i].height = (float)glyphData.texHeight;
            }
        }

        m_fonts["text"] = textFont;
    }
}

Font AssetManager::GetFont(const std::string& name) const {
    auto it = m_fonts.find(name);
    if (it != m_fonts.end()) {
        return it->second;
    }
    return GetFontDefault();
}

void AssetManager::UnloadAssets() {
    for (auto& texture : m_textures) {
        UnloadTexture(texture.second);
    }
    for (auto& sound : m_sounds) {
        UnloadSound(sound.second);
    }
    m_textures.clear();
    m_sounds.clear();
    
    // Descargar fuentes
    for (auto& font : m_fonts) {
        UnloadFont(font.second);
    }
    m_fonts.clear();
    
    // Unload all music
    for (const auto& [name, music] : m_music) {
        ::UnloadMusicStream(music);
    }
    m_music.clear();
    
    CloseAudioDevice();
}

Texture2D AssetManager::GetTexture(const std::string& name) const {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        return it->second;
    }
    return { 0 }; // Return empty texture if not found
}

Sound AssetManager::GetSound(const std::string& name) const {
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        return it->second;
    }
    return { 0 };
}

Rectangle AssetManager::GetTextureRegion(const std::string& name, int x, int y, int width, int height) const {
    return Rectangle{ (float)x, (float)y, (float)width, (float)height };
}

void AssetManager::LoadMusic(const std::string& name, const std::string& path) {
    m_music[name] = LoadMusicStream(path.c_str());
}

Music AssetManager::GetMusic(const std::string& name) const {
    auto it = m_music.find(name);
    if (it != m_music.end()) {
        return it->second;
    }
    return { 0 }; // Return invalid music
}

void AssetManager::UnloadMusic(const std::string& name) {
    auto it = m_music.find(name);
    if (it != m_music.end()) {
        ::UnloadMusicStream(it->second);
        m_music.erase(it);
    }
}
