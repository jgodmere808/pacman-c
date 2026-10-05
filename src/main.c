
#include "config.h"

#include "game.h"
#include "texture_map.h"

int main()
{
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;

    InitWindow(screenWidth, screenHeight, "PACMAN");

    InitAudioDevice();

    initTextureMap();
    initGame();

    SetTargetFPS(60);

    bool gameStarted = false;

    while (!WindowShouldClose()) {

        if (!gameStarted) {
            gameStarted = true;
            startGame();
        }

        BeginDrawing();
            ClearBackground(BLACK);

            updateGame();
            drawGame();

        EndDrawing();
    }

    endGame();

    CloseAudioDevice();

    CloseWindow();

    return 0;
}