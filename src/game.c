
#include "game.h"

#define MAX_GAME_SOUNDS 32

typedef enum {
    SOUND_START,
    SOUND_COUNT
} SoundType;

typedef struct {
    Sound sounds[MAX_GAME_SOUNDS];
    Texture2D background;
} Game;

static Game game;

void initGame()
{
    char *path;

    // Load Background
    game.background = textureMap.background;
    SetTextureFilter(game.background, TEXTURE_FILTER_POINT);

    // SOUND_START
    path = "resources/audio/pacman-arcade-start.ogg";
    game.sounds[SOUND_START] = LoadSound(path);
    if (!IsSoundValid(game.sounds[SOUND_START])) {
        fprintf(stderr, "failed to load sound: %s\n", path);
        exit(EXIT_FAILURE);
    }
}

void endGame()
{
    int i;

    for (i = 0; i < SOUND_COUNT; i++) {
        if (IsSoundValid(game.sounds[i])) {
            UnloadSound(game.sounds[i]);
        }
    }
}

void startGame()
{
    PlaySound(game.sounds[SOUND_START]);
}

void updateGame()
{
    return;
}

void drawGame()
{
    // Draw Background
    DrawTextureEx(
        game.background, 
        (Vector2){ 0, 0 },
        0.0f,
        FACTOR,
        WHITE
    );
}
