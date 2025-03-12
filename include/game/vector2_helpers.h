#ifndef VECTOR2_HELPERS_H
#define VECTOR2_HELPERS_H

#include "raylib.h"
#include <cmath> // Add this line

inline Vector2 MakeVector2(float x, float y) {
    return Vector2{x, y};
}

inline Vector2 NormalizeVector2(Vector2 v) {
    float length = sqrtf(v.x * v.x + v.y * v.y);
    if (length > 0) {
        return Vector2{v.x / length, v.y / length};
    }
    return v;
}

inline Vector2 ScaleVector2(Vector2 v, float scale) {
    return Vector2{v.x * scale, v.y * scale};
}

#endif // VECTOR2_HELPERS_H