
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

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) {
            startGame();
        }

        BeginDrawing();
        ClearBackground(BLACK);
        updateGame();
        drawGame();

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
