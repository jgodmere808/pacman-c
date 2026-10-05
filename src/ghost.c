
#include "ghost.h"

#define GHOST_SPEED 35.0f

static MazeDirection oppositeDirection(MazeDirection direction)
{
    switch (direction) {
        case MAZE_LEFT:  return MAZE_RIGHT;
        case MAZE_RIGHT: return MAZE_LEFT;
        case MAZE_UP:    return MAZE_DOWN;
        case MAZE_DOWN:  return MAZE_UP;
    }

    return MAZE_LEFT;
}

static MazePoint ghostCenter(const Ghost *ghost)
{
    return (MazePoint){
        (int)ghost->pos.x + 8,
        (int)ghost->pos.y + 8
    };
}

static void setGhostCenter(Ghost *ghost, MazePoint center)
{
    ghost->pos.x = (float)(center.x - 8);
    ghost->pos.y = (float)(center.y - 8);
}

/*
 * The ghost house is not included in mazeTryStep's corridor map.
 * Move a ghost to the middle of the house, then up to the mapped
 * corridor at center (112, 116).
 */
static void releaseGhost(Ghost *ghost)
{
    MazePoint center = ghostCenter(ghost);

    if (center.x < 112) {
        center.x++;
        ghost->direction = MAZE_RIGHT;
    } else if (center.x > 112) {
        center.x--;
        ghost->direction = MAZE_LEFT;
    } else if (center.y > 116) {
        center.y--;
        ghost->direction = MAZE_UP;
    }

    setGhostCenter(ghost, center);

    if (center.x == 112 && center.y == 116) {
        ghost->inHouse = false;
    }
}

Ghost initGhost(GhostName name, Vector2 pos)
{
    MazeDirection startDirection = (MazeDirection)GetRandomValue(0, 3);

    Ghost ghost = {
        .name = name,
        .animationState = GHOST_ANIMATION_NORMAL,
        .animationTimer = 0.0f,
        .movementAccumulator = 0.0f,
        .pos = pos,
        .direction = startDirection,
        .inHouse = name != GHOST_BLINKY
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

static void stepGhost(Ghost *ghost)
{
    if (ghost->inHouse) {
        releaseGhost(ghost);
        return;
    }

    MazePoint center = ghostCenter(ghost);
    MazePoint next;

    const MazeDirection directions[] = {
        MAZE_LEFT, MAZE_RIGHT, MAZE_UP, MAZE_DOWN
    };

    MazeDirection choices[4];
    int choiceCount = 0;
    MazeDirection reverse = oppositeDirection(ghost->direction);

    /*
     * Gather legal moves, excluding a reversal. In a straight
     * corridor this leaves one choice; at a junction it leaves
     * two or three.
     */
    for (int i = 0; i < 4; i++) {
        MazeDirection direction = directions[i];

        if (direction != reverse &&
            mazeTryStep(center, direction, &next)) {
            choices[choiceCount++] = direction;
        }
    }

    MazeDirection chosen;

    if (choiceCount > 0) {
        chosen = choices[GetRandomValue(0, choiceCount - 1)];
    } else {
        /* A dead end requires turning around. */
        chosen = reverse;

        if (!mazeTryStep(center, chosen, &next)) {
            return;
        }
    }

    if (!mazeTryStep(center, chosen, &next)) {
        return;
    }

    ghost->direction = chosen;
    setGhostCenter(ghost, next);
}

void updateGhost(Ghost *ghost)
{
    ghost->movementAccumulator += GetFrameTime() * GHOST_SPEED;

    while (ghost->movementAccumulator >= 1.0f) {
        stepGhost(ghost);
        ghost->movementAccumulator -= 1.0f;
    }
}

void drawGhost(Ghost *ghost)
{
    int phase, directionFrame, frame;

    ghost->animationTimer += GetFrameTime();
    phase = (int)(ghost->animationTimer / 0.2f) % 2;

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