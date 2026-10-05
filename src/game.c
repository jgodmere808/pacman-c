
#include "game.h"

#define MAX_GAME_SOUNDS 32

typedef enum {
    SOUND_START,
    SOUND_COUNT
} SoundType;

typedef struct {
    Sound sounds[MAX_GAME_SOUNDS];
} Game;

static Game game;

void initGame()
{
    // SOUND_START
    char *soundPath = "resources/audio/pacman-arcade-start.ogg";
    game.sounds[SOUND_START] = LoadSound(soundPath);
    if (!IsSoundValid(game.sounds[SOUND_START])) {
        fprintf(stderr, "failed to load sound: %s\n", soundPath);
        exit(1);
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
    return;
}
