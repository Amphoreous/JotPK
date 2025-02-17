/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"

<<<<<<< Updated upstream
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
=======
typedef enum { MENU, GAME, HTP } GameScreen;
>>>>>>> Stashed changes

//int main ()
//{
//	// Tell the window to use vsync and work on high DPI displays
//	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
//
//	// Create the window and OpenGL context
//	InitWindow(1280, 800, "Hello Raylib");
//
//	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
//	SearchAndSetResourceDir("resources");
//
//	// Load a texture from the resources directory
//	Texture wabbit = LoadTexture("wabbit_alpha.png");
//	
//	// game loop
//	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
//	{
//		// drawing
//		BeginDrawing();
//
//		// Setup the back buffer for drawing (clear color and depth buffers)
//		ClearBackground(BLACK);
//
//		// draw some text using the default font
//		DrawText("Hello Raylib", 200,200,20,WHITE);
//
//		// draw our texture to the screen
//		DrawTexture(wabbit, 400, 200, WHITE);
//		
//		// end the frame and get ready for the next one  (display frame, poll input, etc...)
//		EndDrawing();
//	}
//
//	// cleanup
//	// unload our texture so it can be cleaned up
//	UnloadTexture(wabbit);
//
//	// destroy the window and cleanup the OpenGL context
//	CloseWindow();
//	return 0;
//}

int main(void) 
{
	// Initialization
	SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MAXIMIZED); // Set window configuration state using flags
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Journey of the Praire King"); // Initialize window and OpenGL context

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
    SearchAndSetResourceDir("resources");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)

    Image image = LoadImage("JotPK.png");     // Loaded in CPU memory (RAM)
    Texture2D texture = LoadTextureFromImage(image);          // Image converted to texture, GPU memory (VRAM)
    UnloadImage(image);   // Once image has been converted to texture and uploaded to VRAM, it can be unloaded from RAM




    //Para que el juego vaya a 60 FPS

    SetTargetFPS(60);       
    

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
<<<<<<< Updated upstream
        // Update
   
        // TODO: Update your variables here

        // Draw

        BeginDrawing();

        ClearBackground(BLACK);

        DrawTexture(texture, 200 / 2 - texture.width / 2, 200 / 2 - texture.height / 2, WHITE);

        EndDrawing();

=======
        if (currentScreen == MENU)
        {
            if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % 3;
            if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption - 1 + 3) % 3;
            if (IsKeyPressed(KEY_ENTER))
            {
                if (selectedOption == 0) currentScreen = GAME; // Iniciar juego
                else if (selectedOption == 1) // Configuración
                {
                
           
                } 
                else if (selectedOption == 2) 
                {
                    currentScreen = HTP;
                }
                else if (selectedOption == 3) break; // Salir
            }

            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(JotPKLogo, (GetScreenWidth() - JotPKLogo.width) / 2, 50, WHITE);
            DrawText("Start", 190, 300, 20, selectedOption == 0 ? RED : WHITE);
            DrawText("Settings", 190, 350, 20, selectedOption == 1 ? RED : WHITE);
            DrawText("How to play", 190, 400, 20, selectedOption == 2 ? RED : WHITE);
            DrawText("Quit", 190, 450, 20, selectedOption == 3 ? RED : WHITE);
            EndDrawing();
        }
        else if (currentScreen == GAME)
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Juego en progreso...", 190, 200, 20, WHITE);
            EndDrawing();
        }
        else if (currentScreen == HTP) {
            ClearBackground(BLACK);
            BeginDrawing();
            DrawText("HOW TO PLAY", GetScreenWidth() / 2, 50, WHITE);
            DrawText("Sasdasdadassdad", 190, 300, 20, WHITE);

            EndDrawing();
        }
>>>>>>> Stashed changes
    }

    // De-Initialization 
    
    //Cierra la ventana y abre OpenGL context

    UnloadTexture(texture);

    CloseWindow();  


    return 0;
}
