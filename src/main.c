/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

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
        // Update
   
        // TODO: Update your variables here

        // Draw

        BeginDrawing();

        ClearBackground(BLACK);

        DrawTexture(texture, 200 / 2 - texture.width / 2, 200 / 2 - texture.height / 2, WHITE);

        EndDrawing();

    }

    // De-Initialization 
    
    //Cierra la ventana y abre OpenGL context

    UnloadTexture(texture);

    CloseWindow();  


    return 0;
}
