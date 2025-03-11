#ifndef MAKEVECTOR2HELPER_H
#define MAKEVECTOR2HELPER_H

#include "raylib.h"

// Helper for Vector2 creation to avoid initialization issues
#if defined(__cplusplus)
inline Vector2 MakeVector2(float x, float y) {
    Vector2 result;
    result.x = x;
    result.y = y;
    return result;
}

// Helper for safe int to float conversion for Vector2
inline Vector2 MakeVector2Int(int x, int y) {
    Vector2 result;
    result.x = (float)x;
    result.y = (float)y;
    return result;
}
#endif

#endif // MAKEVECTOR2HELPER_H