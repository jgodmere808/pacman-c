
#include "ghost.h"

Ghost initGhost(GhostName name, Vector2 pos)
{
    MazeDirection startDirection = (MazeDirection)GetRandomValue(0, 3);

    Ghost ghost = {
        .name = name,
        .animationState = GHOST_ANIMATION_NORMAL,
        .animationTimer = 0.0f,
        .pos = pos,
        .direction = startDirection,
        .requestedDirection = startDirection
    };

    // load appropriate texture
    switch (name) {
        case GHOST_BLINKY:
            ghost.texture = textureMap.blinky;
            break;
        case GHOST_PINKY:
            ghost.texture = textureMap.pinky;
            break;
        case GHOST_INKY:
            ghost.texture = textureMap.inky;
            break;
        case GHOST_CLYDE:
            ghost.texture = textureMap.clyde;
            break;
    }

    return ghost;
}

void updateGhost(Ghost *ghost)
{
    return;
}

void drawGhost(Ghost *ghost)
{
    int phase, directionFrame, frame;

    ghost->animationTimer += GetFrameTime();
    phase = (int)(ghost->animationTimer / 0.2f) % 2;

    directionFrame;
    switch (ghost->direction) {
        case MAZE_RIGHT: directionFrame = 0; break;
        case MAZE_DOWN:  directionFrame = 2; break;
        case MAZE_LEFT:  directionFrame = 4; break;
        case MAZE_UP:    directionFrame = 6; break;
        default:         directionFrame = 0; break;
    }

    switch (ghost->animationState) {
        case GHOST_ANIMATION_EYES:
            frame = 12 + directionFrame / 2;
            break;
        case GHOST_ANIMATION_BLINKING:
            frame = 10 + phase;
            break;
        case GHOST_ANIMATION_NORMAL:
        default:
            frame = directionFrame + phase;
            break;
    }

    DrawTexturePro(
        ghost->texture,
        (Rectangle){ frame * 16, 0, 16, 16 },
        (Rectangle){
            ghost->pos.x * FACTOR,
            ghost->pos.y * FACTOR,
            16 * FACTOR,
            16 * FACTOR
        },
        (Vector2){ 0, 0 },
        0.0f,
        WHITE
    );
}