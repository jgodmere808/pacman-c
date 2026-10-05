
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

        if (!gameStarted && IsKeyPressed(KEY_SPACE)) {
            gameStarted = true;
            startGame();
        }

        BeginDrawing();
            ClearBackground(BLACK);

            updateGame();
            drawGame();

        if (!gameStarted) {
            DrawText("PRESS SPACE TO START", 60, 400, 42, WHITE);
        }

        EndDrawing();
    }

    endGame();

    UnloadTexture(textureMap.pacman);
    UnloadTexture(textureMap.background);
    UnloadTexture(textureMap.dotSmall);
    UnloadTexture(textureMap.dotLarge);

    CloseAudioDevice();

    CloseWindow();

    return 0;
}
