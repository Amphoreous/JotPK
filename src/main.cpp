#include "raylib.h"
#include "resource_dir.h"
#include "GameApplication.hpp"

int main() {
    GameApplication app;
    app.Initialize();
    app.Run();
    return 0;
}
