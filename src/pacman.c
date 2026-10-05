
#include "pacman.h"

#define PACMAN_SPEED 20.0f
#define PACMAN_ANIMATION_TIME_EATING 0.20f

Pacman initPacman(Vector2 pos)
{
    Pacman pacman = {
        .texture = textureMap.pacman,
        .animationState = IDLE,
        .animationTimer = 0,
        .pos = pos,
        .vel = (Vector2){ 1, 0 },
        .nextVel = (Vector2){ 1, 0 }
    };

    return pacman;
}

void updatePacman(Pacman *pacman)
{
    float nextX, nextY;

    if (IsKeyPressed(KEY_LEFT) && !IsKeyPressed(KEY_RIGHT)) {
        // left
        pacman->nextVel = (Vector2){ -1, 0 };
    }
    if (!IsKeyPressed(KEY_LEFT) && IsKeyPressed(KEY_RIGHT)) {
        // right
        pacman->nextVel = (Vector2){ 1, 0 };
    }
    if (IsKeyPressed(KEY_UP) && !IsKeyPressed(KEY_DOWN)) {
        // up
        pacman->nextVel = (Vector2){ 0, -1 };
    }
    if (!IsKeyPressed(KEY_UP) && IsKeyPressed(KEY_DOWN)) {
        // down
        pacman->nextVel = (Vector2){ 0, 1 };
    }

    if (/* !collisions */ 1) {
        pacman->vel = pacman->nextVel;
    }

    // determine which direction pacman is facing
    if (pacman->vel.x == -1 && pacman->vel.y == 0) {
        // left
        pacman->animationState = FACING_LEFT;
    } else if (pacman->vel.x == 1 && pacman->vel.y == 0) {
        // right
        pacman->animationState = FACING_RIGHT;
    } else if (pacman->vel.x == 0 && pacman->vel.y == -1) {
        // up
        pacman->animationState = FACING_UP;
    } else if (pacman->vel.x == 0 && pacman->vel.y == 1) {
        // down
        pacman->animationState = FACING_DOWN;
    } else {
        fprintf(
            stderr,
            "Invalid pacman velocity: { %.2f, %.2f }\n",
            pacman->vel.x,
            pacman->vel.y
        );
        exit(EXIT_FAILURE);
    }

    nextX = pacman->vel.x + pacman->pos.x;
    nextY = pacman->vel.y + pacman->pos.y;

    // update pacman position
    pacman->pos = (Vector2){ nextX, nextY };
}

void drawPacman(Pacman *pacman)
{
    int frame, frameIndex;
    int frameConverter[4];
    Rectangle source;

    pacman->animationTimer += GetFrameTime();
    while (pacman->animationTimer >= PACMAN_ANIMATION_TIME_EATING) {
        pacman->animationTimer -= PACMAN_ANIMATION_TIME_EATING;
    }

    switch (pacman->animationState) {
        case IDLE:
            frame = 4;
            source = (Rectangle){ frame * 16, 0, 16, 16 };
            break;
        case FACING_LEFT:
            frameConverter[0] = 4;
            frameConverter[1] = 2;
            frameConverter[2] = 0;
            frameConverter[3] = 2;
            frameIndex = (int)(pacman->animationTimer / (PACMAN_ANIMATION_TIME_EATING / 4)) % 4;
            frame = frameConverter[frameIndex];
            source = (Rectangle){ frame * 16, 0, -16, 16 };
            break;
        case FACING_RIGHT:
            frameConverter[0] = 4;
            frameConverter[1] = 2;
            frameConverter[2] = 0;
            frameConverter[3] = 2;
            frameIndex = (int)(pacman->animationTimer / (PACMAN_ANIMATION_TIME_EATING / 4)) % 4;
            frame = frameConverter[frameIndex];
            source = (Rectangle){ frame * 16, 0, 16, 16 };
            break;
        case FACING_UP:
            frameConverter[0] = 4;
            frameConverter[1] = 3;
            frameConverter[2] = 1;
            frameConverter[3] = 3;
            frameIndex = (int)(pacman->animationTimer / (PACMAN_ANIMATION_TIME_EATING / 4)) % 4;
            frame = frameConverter[frameIndex];
            source = (Rectangle){ frame * 16, 0, 16, -16 };
            break;
        case FACING_DOWN:
            frameConverter[0] = 4;
            frameConverter[1] = 3;
            frameConverter[2] = 1;
            frameConverter[3] = 3;
            frameIndex = (int)(pacman->animationTimer / (PACMAN_ANIMATION_TIME_EATING / 4)) % 4;
            frame = frameConverter[frameIndex];
            source = (Rectangle){ frame * 16, 0, 16, 16 };
            break;
        default:
            fprintf(stderr, "Invalid pacman->animationState: %i\n", pacman->animationState);
            exit(EXIT_FAILURE);
    }

    DrawTexturePro(
        pacman->texture,
        source,
        (Rectangle){
            pacman->pos.x * FACTOR,
            pacman->pos.y * FACTOR,
            16 * FACTOR,
            16 * FACTOR
        },
        (Vector2){ 0, 0 },
        0.0f,
        WHITE
    );
}
