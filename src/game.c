
#include "game.h"
#include "game_audio.h"
#include "dots.h"

#define STARTING_LIVES 3
#define COLLISION_DISTANCE 8
#define FRIGHTENED_SECONDS 6.0f

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
    SOUND_DEATH,
    SOUND_COUNT
} SoundType;

typedef struct {
    Pacman pacman;
    Ghost ghosts[4];
    Dots dots;
    Sound sounds[SOUND_COUNT];
    Texture2D background;
    int score;
    int lives;
    int nextEatSound;
    int frightenedGhostsEaten;
    float frightenedTimeLeft;
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
    game.frightenedTimeLeft = 0.0f;
    game.frightenedGhostsEaten = 0;
    game.pacman = initPacman((Vector2){ 104, 204 });

    game.ghosts[GHOST_BLINKY] = initGhost(GHOST_BLINKY, (Vector2){ 104, 108 });
    game.ghosts[GHOST_PINKY]  = initGhost(GHOST_PINKY,  (Vector2){ 104, 132 });
    game.ghosts[GHOST_INKY]   = initGhost(GHOST_INKY,   (Vector2){ 88,  132 });
    game.ghosts[GHOST_CLYDE]  = initGhost(GHOST_CLYDE,  (Vector2){ 120, 132 });
}

static bool touchesGhost(const Ghost *ghost)
{
    int pacmanX = (int)game.pacman.pos.x + 8;
    int pacmanY = (int)game.pacman.pos.y + 8;
    int dx = pacmanX - ((int)ghost->pos.x + 8);
    int dy = pacmanY - ((int)ghost->pos.y + 8);

    return dx * dx + dy * dy <=
        COLLISION_DISTANCE * COLLISION_DISTANCE;
}

/* Resolve lethal overlaps first when Pac-Man touches multiple ghosts. */
static bool resolveGhostCollisions(void)
{
    bool ateGhost = false;

    for (int i = 0; i < 4; i++) {
        Ghost *ghost = &game.ghosts[i];

        if (ghost->mode == GHOST_NORMAL && touchesGhost(ghost)) {
            return true;
        }
    }

    for (int i = 0; i < 4; i++) {
        Ghost *ghost = &game.ghosts[i];

        if (ghost->mode != GHOST_FRIGHTENED ||
            !touchesGhost(ghost)) {
            continue;
        }

        int chainIndex = game.frightenedGhostsEaten;

        if (chainIndex > 3) {
            chainIndex = 3;
        }

        addScore(200 << chainIndex);
        game.frightenedGhostsEaten++;
        ghostStartReturning(ghost);
        ateGhost = true;
    }

    if (ateGhost) {
        playGhostEatenSound();
    }

    return false;
}

static void startFrightened(void)
{
    game.frightenedTimeLeft = FRIGHTENED_SECONDS;
    game.frightenedGhostsEaten = 0;

    for (int i = 0; i < 4; i++) {
        ghostStartFrightened(&game.ghosts[i]);
    }
}

static void updateFrightenedTimer(float deltaTime)
{
    if (game.frightenedTimeLeft <= 0.0f) {
        return;
    }

    game.frightenedTimeLeft -= deltaTime;

    if (game.frightenedTimeLeft > 0.0f) {
        return;
    }

    game.frightenedTimeLeft = 0.0f;
    game.frightenedGhostsEaten = 0;

    for (int i = 0; i < 4; i++) {
        ghostEndFrightened(&game.ghosts[i]);
    }
}

void initGame()
{
    game.score = 0;
    game.lives = STARTING_LIVES;
    game.nextEatSound = 0;
    game.state = GAME_PAUSED;
    resetActors();

    initDots(&game.dots);

    // Load Background
    game.background = textureMap.background;
    SetTextureFilter(game.background, TEXTURE_FILTER_POINT);

    initGameAudio();

    game.sounds[SOUND_START] =
        loadGameSound("resources/audio/pacman-arcade-start.wav");
    game.sounds[SOUND_EAT_0] =
        loadGameSound("resources/audio/pacman-arcade-eat-dot-0.wav");
    game.sounds[SOUND_EAT_1] =
        loadGameSound("resources/audio/pacman-arcade-eat-dot-1.wav");
    game.sounds[SOUND_DEATH] =
        loadGameSound("resources/audio/pacman-arcade-death.wav");
}

void endGame()
{
    int i;

    for (i = 0; i < SOUND_COUNT; i++) {
        if (IsSoundValid(game.sounds[i])) {
            UnloadSound(game.sounds[i]);
        }
    }

    endGameAudio();
}

void startGame()
{
    if (game.state != GAME_PAUSED && game.state != GAME_OVER) {
        return;
    }

    if (game.state == GAME_OVER) {
        stopGameAudio();
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

        if (!IsSoundPlaying(game.sounds[SOUND_DEATH]) &&
            pacmanDeathFinished(&game.pacman)) {
            if (game.lives == 0) {
                game.state = GAME_OVER;
            } else {
                resetActors();
                game.state = GAME_IN_PROGRESS;
            }
        }
        return;
    }

    if (game.state != GAME_IN_PROGRESS) {
        return;
    }

    float deltaTime = GetFrameTime();

    updateFrightenedTimer(deltaTime);

    for (int i = 0; i < 4; i++) {
        updateGhost(
            &game.ghosts[i],
            deltaTime,
            game.frightenedTimeLeft
        );
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

        /* Apply the power pellet before checking collisions. */
        if (collected == DOT_LARGE) {
            startFrightened();
        }
    }

    if (collected != DOT_NONE && game.dots.remaining == 0) {
        stopGameAudio();
        initDots(&game.dots);
        resetActors();
        PlaySound(game.sounds[SOUND_START]);
        game.state = GAME_STARTING;
        return;
    }

    if (resolveGhostCollisions()) {
        game.lives--;
        startPacmanDeath(&game.pacman);
        game.state = GAME_DYING;
        stopGameAudio();
        StopSound(game.sounds[SOUND_EAT_0]);
        StopSound(game.sounds[SOUND_EAT_1]);
        PlaySound(game.sounds[SOUND_DEATH]);
        return;
    }

    bool eyesReturning = false;

    for (int i = 0; i < 4; i++) {
        if (game.ghosts[i].mode == GHOST_EYES_RETURNING) {
            eyesReturning = true;
            break;
        }
    }

    updateGameAudio(game.frightenedTimeLeft > 0.0f, eyesReturning);
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
            drawGhost(&game.ghosts[i], game.frightenedTimeLeft);
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
