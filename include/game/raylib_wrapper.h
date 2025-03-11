#ifndef RAYLIB_WRAPPER_H
#define RAYLIB_WRAPPER_H

// Include raylib headers but wrap Vector2Distance and Clamp to avoid conflicts
#include "raylib.h"
#include <cmath>

#ifdef __cplusplus
extern "C" {
#endif

// Only define these if they're not already defined
#ifndef Vector2Distance
inline float Vector2Distance(Vector2 v1, Vector2 v2) {
    return sqrtf((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y));
}
#endif

#ifndef Clamp
inline float Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
#endif

#ifdef __cplusplus
}
#endif

#endif // RAYLIB_WRAPPER_H