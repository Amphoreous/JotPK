 #ifndef MENU_H
#define MENU_H

#include "raylib.h"

typedef enum { MENU, GAME, SETTINGS, HTP } GameScreen;

void DrawMenu(Texture JotPKLogo, int selectedOption);
void UpdateMenu(GameScreen* currentScreen, int* selectedOption);

#endif // MENU_H