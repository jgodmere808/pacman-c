
#include "game.h"
#include "dots.h"

#define STARTING_LIVES 3
#define COLLISION_DISTANCE 8

typedef enum {
    GAME_PAUSED,
    GAME_STARTING,
    GAME_IN_PROGRESS,
    GAME_DYING,
    GAME_OVER
} GameState;

typedef enum {
    SOUND_START,
    SOUND_EAT_0,
    SOUND_EAT_1,
    SOUND_DEATH_0,
    SOUND_DEATH_1,
    SOUND_COUNT
} SoundType;

typedef struct {
    Pacman pacman;
    Ghost ghosts[4];
    Dots dots;
    Sound sounds[SOUND_COUNT];
    Music backgroundMusic;
    bool backgroundMusicStarted;
    bool deathFinalStarted;
    Texture2D background;
    int score;
    int lives;
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

static void resetActors(void)
{
    game.pacman = initPacman((Vector2){ 104, 204 });

    game.ghosts[GHOST_BLINKY] = initGhost(GHOST_BLINKY, (Vector2){ 104, 108 });
    game.ghosts[GHOST_PINKY]  = initGhost(GHOST_PINKY,  (Vector2){ 104, 132 });
    game.ghosts[GHOST_INKY]   = initGhost(GHOST_INKY,   (Vector2){ 88,  132 });
    game.ghosts[GHOST_CLYDE]  = initGhost(GHOST_CLYDE,  (Vector2){ 120, 132 });
}

static bool touchesNormalGhost(void)
{
    int pacmanX = (int)game.pacman.pos.x + 8;
    int pacmanY = (int)game.pacman.pos.y + 8;
    int limitSquared = COLLISION_DISTANCE * COLLISION_DISTANCE;

    for (int i = 0; i < 4; i++) {
        const Ghost *ghost = &game.ghosts[i];

        if (ghost->animationState != GHOST_ANIMATION_NORMAL) {
            continue;
        }

        int dx = pacmanX - ((int)ghost->pos.x + 8);
        int dy = pacmanY - ((int)ghost->pos.y + 8);

        if (dx * dx + dy * dy <= limitSquared) {
            return true;
        }
    }

    return false;
}

void initGame()
{
    const char *musicPath = "resources/audio/pacman-arcade-background-music.wav";

    game.score = 0;
    game.lives = STARTING_LIVES;
    game.nextEatSound = 0;
    game.state = GAME_PAUSED;
    game.backgroundMusicStarted = false;
    game.deathFinalStarted = false;
    resetActors();

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
    game.sounds[SOUND_DEATH_0] =
        loadGameSound("resources/audio/pacman-arcade-death-0.wav");
    game.sounds[SOUND_DEATH_1] =
        loadGameSound("resources/audio/pacman-arcade-death-1.wav");
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
    if (game.state != GAME_PAUSED && game.state != GAME_OVER) {
        return;
    }

    if (game.state == GAME_OVER) {
        StopMusicStream(game.backgroundMusic);
        game.backgroundMusicStarted = false;
        game.score = 0;
        game.lives = STARTING_LIVES;
        game.nextEatSound = 0;
        initDots(&game.dots);
        resetActors();
    }

    PlaySound(game.sounds[SOUND_START]);
    game.state = GAME_STARTING;
}

void addScore(int points)
{
    game.score += points;
}

void updateGame()
{
    if (game.state == GAME_STARTING) {
        if (!IsSoundPlaying(game.sounds[SOUND_START])) {
            game.state = GAME_IN_PROGRESS;
        }
        return;
    }

    if (game.state == GAME_DYING) {
        updatePacman(&game.pacman);

        if (!game.deathFinalStarted &&
            !IsSoundPlaying(game.sounds[SOUND_DEATH_0])) {
            PlaySound(game.sounds[SOUND_DEATH_1]);
            game.deathFinalStarted = true;
        }

        if (game.deathFinalStarted &&
            !IsSoundPlaying(game.sounds[SOUND_DEATH_1]) &&
            pacmanDeathFinished(&game.pacman)) {
            if (game.lives == 0) {
                game.state = GAME_OVER;
            } else {
                resetActors();
                ResumeMusicStream(game.backgroundMusic);
                game.state = GAME_IN_PROGRESS;
            }
        }
        return;
    }

    if (game.state != GAME_IN_PROGRESS) {
        return;
    }

    if (!game.backgroundMusicStarted) {
        PlayMusicStream(game.backgroundMusic);
        game.backgroundMusicStarted = true;
    }
    UpdateMusicStream(game.backgroundMusic);

    for (int i = 0; i < 4; i++) {
        updateGhost(&game.ghosts[i]);
    }

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

        /* Apply future power-pellet effects before checking collisions. */
    }

    if (touchesNormalGhost()) {
        game.lives--;
        startPacmanDeath(&game.pacman);
        game.state = GAME_DYING;
        game.deathFinalStarted = false;
        PauseMusicStream(game.backgroundMusic);
        StopSound(game.sounds[SOUND_EAT_0]);
        StopSound(game.sounds[SOUND_EAT_1]);
        PlaySound(game.sounds[SOUND_DEATH_0]);
    }
}

void drawGame()
{
    int i;

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

    // Show number of lives left
    const Rectangle lifeIcon = { 2 * 16, 0, 16, 16 };
    for (int life = 0; life < game.lives; life++) {
        DrawTexturePro(
            textureMap.pacman,
            lifeIcon,
            (Rectangle){
                (6 + life * 15) * FACTOR,
                273 * FACTOR,
                12 * FACTOR,
                12 * FACTOR
            },
            (Vector2){ 0, 0 },
            0.0f,
            WHITE
        );
    }

    if (game.state != GAME_DYING && game.state != GAME_OVER) {
        for (i = 0; i < 4; i++) {
            drawGhost(&game.ghosts[i]);
        }
    }

    if (game.state != GAME_OVER) {
        drawPacman(&game.pacman);
    }

    if (game.state == GAME_PAUSED) {
        DrawText("PRESS SPACE TO START", 60, 400, 42, WHITE);
    } else if (game.state == GAME_OVER) {
        const char *message = "GAME OVER";
        const char *restart = "PRESS SPACE TO RESTART";

        DrawText(message, (SCREEN_WIDTH - MeasureText(message, 42)) / 2,
                 400, 42, WHITE);
        DrawText(restart, (SCREEN_WIDTH - MeasureText(restart, 24)) / 2,
                 455, 24, WHITE);
    }
}
