
#include "game.h"
#include "dots.h"

#define MAX_GAME_SOUNDS 32

typedef enum {
    GAME_PAUSED,
    GAME_STARTING,
    GAME_IN_PROGRESS
} GameState;

typedef enum {
    SOUND_START,
    SOUND_EAT_0,
    SOUND_EAT_1,
    SOUND_COUNT
} SoundType;

typedef struct {
    Pacman pacman;
    Dots dots;
    Sound sounds[MAX_GAME_SOUNDS];
    Music backgroundMusic;
    bool backgroundMusicStarted;
    Texture2D background;
    int score;
    int nextEatSound;
    GameState state;
} Game;

static Game game;

static Sound loadGameSound(const char *path)
{
    Sound sound = LoadSound(path);

    if (!IsSoundValid(sound)) {
        fprintf(stderr, "failed to load sound: %s\n", path);
        exit(EXIT_FAILURE);
    }

    return sound;
}

void initGame()
{
    char *musicPath = "resources/audio/pacman-arcade-background-music.wav";

    game.score = 0;
    game.nextEatSound = 0;
    game.state = GAME_PAUSED;
    game.backgroundMusicStarted = false;
    game.pacman = initPacman((Vector2){ 104, 204 });

    initDots(&game.dots);

    // Load Background
    game.background = textureMap.background;
    SetTextureFilter(game.background, TEXTURE_FILTER_POINT);

    // Load Background Music
    game.backgroundMusic = LoadMusicStream(musicPath);
    if (!IsMusicValid(game.backgroundMusic)) {
        fprintf(stderr, "failed to load background music: %s\n", musicPath);
        exit(EXIT_FAILURE);
    }
    game.backgroundMusic.looping = true;

    game.sounds[SOUND_START] =
        loadGameSound("resources/audio/pacman-arcade-start.wav");
    game.sounds[SOUND_EAT_0] =
        loadGameSound("resources/audio/pacman-arcade-eat-dot-0.wav");
    game.sounds[SOUND_EAT_1] =
        loadGameSound("resources/audio/pacman-arcade-eat-dot-1.wav");
}

void endGame()
{
    int i;

    for (i = 0; i < SOUND_COUNT; i++) {
        if (IsSoundValid(game.sounds[i])) {
            UnloadSound(game.sounds[i]);
        }
    }

    UnloadMusicStream(game.backgroundMusic);
}

void startGame()
{
    PlaySound(game.sounds[SOUND_START]);
    game.state = GAME_STARTING;
}

void addScore(int points)
{
    game.score += points;
}

void updateGame()
{
    // Wait for starting music to complete
    if (game.state == GAME_STARTING && !IsSoundPlaying(game.sounds[SOUND_START])) {
        game.state = GAME_IN_PROGRESS;
    }

    // Start background music when game begins
    if (!game.backgroundMusicStarted && game.state == GAME_IN_PROGRESS) {
        PlayMusicStream(game.backgroundMusic);
        game.backgroundMusicStarted = true;
    }

    // update background music
    if (game.backgroundMusicStarted && game.state == GAME_IN_PROGRESS) {
        UpdateMusicStream(game.backgroundMusic);
    }

    // game is in progress
    if (game.state == GAME_IN_PROGRESS) {
        updatePacman(&game.pacman);

        MazePoint center = {
            (int)game.pacman.pos.x + 8,
            (int)game.pacman.pos.y + 8
        };

        DotType collected = collectDot(&game.dots, center);

        if (collected != DOT_NONE) {
            addScore(collected == DOT_LARGE ? 50 : 10);

            PlaySound(game.sounds[SOUND_EAT_0 + game.nextEatSound]);

            game.nextEatSound ^= 1;

            /* A large dot can trigger frightened mode here
               when ghosts are added. */
        }
    }
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

    drawDots(&game.dots);

    // Draw Score
    const int fontSize = 8 * FACTOR;
    const char *label = "SCORE";
    const char *value = TextFormat("%02d", game.score);

    DrawText(label, (SCREEN_WIDTH - MeasureText(label, fontSize)) / 2,
             2 * FACTOR, fontSize, WHITE);
    DrawText(value, (SCREEN_WIDTH - MeasureText(value, fontSize)) / 2,
             13 * FACTOR, fontSize, WHITE);

    // Draw Pacman
    drawPacman(&game.pacman);
}
