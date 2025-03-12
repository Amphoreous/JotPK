#ifndef RAYLIB_WRAPPER_H
#define RAYLIB_WRAPPER_H

#include "raylib.h"

// Only include definitions if raymath.h and its common variants are not included
#if !defined(RAYMATH_H) && !defined(_RAYMATH_H_) && !defined(__RAYMATH_H__)
#include <cmath>

// Remove duplicate definitions
/*
#ifndef Vector2Distance
inline float Vector2Distance(Vector2 v1, Vector2 v2) {
    return sqrtf((v2.x - v1.x)*(v2.x - v1.x) + (v2.y - v1.y)*(v2.y - v1.y));
}
#endif

#ifndef Clamp
inline float Clamp(float value, float min, float max) {
    return (value < min) ? min : ((value > max) ? max : value);
}
#endif
*/

#endif // !defined(RAYMATH_H) ...

#endif // RAYLIB_WRAPPER_H