
#include "game.h"

#define MAX_GAME_SOUNDS 32

typedef enum {
    SOUND_START,
    SOUND_COUNT
} SoundType;

typedef struct {
    Sound sounds[MAX_GAME_SOUNDS];
    Texture2D background;
    int score;
} Game;

static Game game;

void initGame()
{
    char *path;

    game.score = 0;

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

void addScore(int points)
{
    game.score += points;
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


    // Draw Score
    const int fontSize = 8 * FACTOR;
    const char *label = "SCORE";
    const char *value = TextFormat("%02d", game.score);

    DrawText(label, (SCREEN_WIDTH - MeasureText(label, fontSize)) / 2,
             2 * FACTOR, fontSize, WHITE);
    DrawText(value, (SCREEN_WIDTH - MeasureText(value, fontSize)) / 2,
             13 * FACTOR, fontSize, WHITE);
}
