#ifndef RAYLIB_WRAPPER_H
#define RAYLIB_WRAPPER_H

// Include raylib headers but conditionally define helper functions
#include "raylib.h"
#include "raymath.h"  // Include raymath directly to use its functions
#include <cmath>

// Remove the conflicting function definitions that were causing errors
// Let raymath.h handle these functions

#endif // RAYLIB_WRAPPER_H