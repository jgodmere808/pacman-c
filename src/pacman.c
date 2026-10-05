
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
        .direction = MAZE_RIGHT,
        .requestedDirection = MAZE_RIGHT
    };

    return pacman;
}

void updatePacman(Pacman *pacman)
{
    if (IsKeyPressed(KEY_LEFT)) {
        pacman->requestedDirection = MAZE_LEFT;
    } else if (IsKeyPressed(KEY_RIGHT)) {
        pacman->requestedDirection = MAZE_RIGHT;
    } else if (IsKeyPressed(KEY_UP)) {
        pacman->requestedDirection = MAZE_UP;
    } else if (IsKeyPressed(KEY_DOWN)) {
        pacman->requestedDirection = MAZE_DOWN;
    }

    MazePoint center = {
        (int)pacman->pos.x + 8,
        (int)pacman->pos.y + 8
    };

    MazePoint next;

    /*
     * First try the buffered turn. If it is unavailable, continue
     * in the current direction. If both fail, stay in place.
     */
    if (mazeTryStep(center, pacman->requestedDirection, &next)) {
        pacman->direction = pacman->requestedDirection;
    } else if (!mazeTryStep(center, pacman->direction, &next)) {
        return;
    }

    pacman->pos.x = (float)(next.x - 8);
    pacman->pos.y = (float)(next.y - 8);

    // determine which direction pacman is facing
    switch (pacman->direction) {
        case MAZE_LEFT:
            pacman->animationState = FACING_LEFT;
            break;
        case MAZE_RIGHT:
            pacman->animationState = FACING_RIGHT;
            break;
        case MAZE_UP:
            pacman->animationState = FACING_UP;
            break;
        case MAZE_DOWN:
            pacman->animationState = FACING_DOWN;
            break;
    }
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
