#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include <map>

struct GlyphData {
    // Posición en la textura
    int texX;
    int texY;
    int texWidth;
    int texHeight;
    
    // Área de recorte/renderizado
    int cropX;
    int cropY;
    int cropWidth;
    int cropHeight;
    
    // Información de kerning
    std::map<char, int> kerning;
};

struct FontData {
    std::map<char, GlyphData> characters;  // Mapa de carácter a su información
    std::vector<std::string> characterMap;  // Array de caracteres en orden
    int verticalLineSpacing;
    int horizontalSpacing;
    std::string defaultCharacter;
};

class JsonHelper {
public:
    static FontData LoadFontData(const std::string& jsonPath) {
        FontData data;
        std::ifstream file(jsonPath);
        if (!file.is_open()) return data;

        nlohmann::json j;
        file >> j;

        auto& content = j["content"];
        data.verticalLineSpacing = content["verticalLineSpacing"];
        data.horizontalSpacing = content["horizontalSpacing"];
        data.defaultCharacter = content["defaultCharacter"];

        auto& glyphs = content["glyphs"];
        auto& cropping = content["cropping"];
        auto& charMap = content["characterMap"];
        auto& kerningData = content["kerning"];

        // Procesar los datos del characterMap
        data.characterMap = content["characterMap"].get<std::vector<std::string>>();

        // Crear mapa de kerning por carácter
        std::map<int, std::map<int, int>> kerningMap;
        for (const auto& k : kerningData) {
            int first = k["x"].get<int>();
            int second = k["y"].get<int>();
            int amount = k["z"].get<int>();
            kerningMap[first][second] = amount;
        }

        // Procesar cada carácter
        for (size_t i = 0; i < charMap.size() && i < glyphs.size() && i < cropping.size(); i++) {
            char c = charMap[i].get<std::string>()[0];
            GlyphData glyph;
            
            // Datos de la textura
            glyph.texX = glyphs[i].value("x", 0);
            glyph.texY = glyphs[i].value("y", 0);
            glyph.texWidth = glyphs[i].value("width", 0);
            glyph.texHeight = glyphs[i]["height"];
            
            // Datos de recorte
            glyph.cropX = cropping[i]["x"];
            glyph.cropY = cropping[i]["y"];
            glyph.cropWidth = cropping[i]["width"];
            glyph.cropHeight = cropping[i]["height"];

            // Añadir kerning para este carácter
            if (kerningMap.count(i) > 0) {
                for (const auto& [second, amount] : kerningMap[i]) {
                    if (second < charMap.size()) {
                        char nextChar = charMap[second].get<std::string>()[0];
                        glyph.kerning[nextChar] = amount;
                    }
                }
            }

            data.characters[c] = glyph;
        }

        return data;
    }
};
