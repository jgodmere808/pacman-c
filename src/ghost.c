#include "ghost.h"

#include <limits.h>

#define GHOST_NORMAL_SPEED 50.0f
#define GHOST_FRIGHTENED_SPEED 35.0f
#define GHOST_EYES_SPEED 100.0f
#define GHOST_REPLENISH_SECONDS 1.5f
#define GHOST_FLASH_SECONDS 2.0f

#define HOUSE_ENTRANCE_X 112
#define HOUSE_ENTRANCE_Y 116
#define HOUSE_CENTER_Y 140

/* mazeTryStep includes the tunnel from x = -8 to x = 232. */
#define ROUTE_MIN_X (-8)
#define ROUTE_MAX_X 232
#define ROUTE_HEIGHT 288
#define ROUTE_WIDTH (ROUTE_MAX_X - ROUTE_MIN_X + 1)

static int returnDistance[ROUTE_HEIGHT][ROUTE_WIDTH];
static MazePoint routeQueue[ROUTE_HEIGHT * ROUTE_WIDTH];
static bool returnRoutesReady = false;

static const MazeDirection directions[] = {
    MAZE_LEFT, MAZE_RIGHT, MAZE_UP, MAZE_DOWN
};

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

static bool inRouteBounds(MazePoint point)
{
    return point.x >= ROUTE_MIN_X &&
           point.x <= ROUTE_MAX_X &&
           point.y >= 0 &&
           point.y < ROUTE_HEIGHT;
}

static int *distanceAt(MazePoint point)
{
    return &returnDistance[point.y][point.x - ROUTE_MIN_X];
}

/* A single BFS distance map gives every returning ghost a route home. */
static void buildReturnRoutes(void)
{
    for (int y = 0; y < ROUTE_HEIGHT; y++) {
        for (int x = 0; x < ROUTE_WIDTH; x++) {
            returnDistance[y][x] = -1;
        }
    }

    MazePoint entrance = { HOUSE_ENTRANCE_X, HOUSE_ENTRANCE_Y };
    int head = 0;
    int tail = 0;

    *distanceAt(entrance) = 0;
    routeQueue[tail++] = entrance;

    while (head < tail) {
        MazePoint current = routeQueue[head++];
        int currentDistance = *distanceAt(current);

        for (int i = 0; i < 4; i++) {
            MazePoint next;

            if (!mazeTryStep(current, directions[i], &next) ||
                !inRouteBounds(next) ||
                *distanceAt(next) != -1) {
                continue;
            }

            *distanceAt(next) = currentDistance + 1;
            routeQueue[tail++] = next;
        }
    }

    returnRoutesReady = true;
}

/* The house interior is outside mazeTryStep's corridor map. */
static void releaseGhost(Ghost *ghost, float frightenedTimeLeft)
{
    MazePoint center = ghostCenter(ghost);

    if (center.x < HOUSE_ENTRANCE_X) {
        center.x++;
        ghost->direction = MAZE_RIGHT;
    } else if (center.x > HOUSE_ENTRANCE_X) {
        center.x--;
        ghost->direction = MAZE_LEFT;
    } else if (center.y > HOUSE_ENTRANCE_Y) {
        center.y--;
        ghost->direction = MAZE_UP;
    }

    setGhostCenter(ghost, center);

    if (center.x == HOUSE_ENTRANCE_X &&
        center.y == HOUSE_ENTRANCE_Y) {
        ghost->mode = frightenedTimeLeft > 0.0f
            ? GHOST_FRIGHTENED
            : GHOST_NORMAL;
    }
}

Ghost initGhost(GhostName name, Vector2 pos)
{
    if (!returnRoutesReady) {
        buildReturnRoutes();
    }

    Ghost ghost = {
        .name = name,
        .mode = name == GHOST_BLINKY
            ? GHOST_NORMAL
            : GHOST_EXITING_HOUSE,
        .animationTimer = 0.0f,
        .movementAccumulator = 0.0f,
        .replenishTimer = 0.0f,
        .pos = pos,
        .direction = (MazeDirection)GetRandomValue(0, 3)
    };

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

void ghostStartFrightened(Ghost *ghost)
{
    if (ghost->mode == GHOST_NORMAL ||
        ghost->mode == GHOST_FRIGHTENED) {
        ghost->mode = GHOST_FRIGHTENED;
        ghost->direction = oppositeDirection(ghost->direction);
        ghost->animationTimer = 0.0f;
    }
}

void ghostEndFrightened(Ghost *ghost)
{
    if (ghost->mode == GHOST_FRIGHTENED) {
        ghost->mode = GHOST_NORMAL;
    }
}

void ghostStartReturning(Ghost *ghost)
{
    if (ghost->mode == GHOST_FRIGHTENED) {
        ghost->mode = GHOST_EYES_RETURNING;
        ghost->movementAccumulator = 0.0f;
    }
}

static void stepRoaming(Ghost *ghost)
{
    MazePoint center = ghostCenter(ghost);
    MazePoint next;
    MazeDirection choices[4];
    int choiceCount = 0;
    MazeDirection reverse = oppositeDirection(ghost->direction);

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
        chosen = reverse;

        if (!mazeTryStep(center, chosen, &next)) {
            return;
        }
    }

    if (mazeTryStep(center, chosen, &next)) {
        ghost->direction = chosen;
        setGhostCenter(ghost, next);
    }
}

static void stepReturning(Ghost *ghost)
{
    MazePoint center = ghostCenter(ghost);

    /* Pass through the unmapped house doorway after reaching it. */
    if (center.x == HOUSE_ENTRANCE_X &&
        center.y >= HOUSE_ENTRANCE_Y &&
        center.y < HOUSE_CENTER_Y) {
        center.y++;
        ghost->direction = MAZE_DOWN;
        setGhostCenter(ghost, center);

        if (center.y == HOUSE_CENTER_Y) {
            ghost->mode = GHOST_REPLENISHING;
            ghost->replenishTimer = GHOST_REPLENISH_SECONDS;
            ghost->movementAccumulator = 0.0f;
        }

        return;
    }

    MazePoint bestNext = center;
    MazeDirection bestDirection = ghost->direction;
    int bestDistance = INT_MAX;

    /* Eyes may reverse direction; take a shortest legal step. */
    for (int i = 0; i < 4; i++) {
        MazePoint next;

        if (!mazeTryStep(center, directions[i], &next) ||
            !inRouteBounds(next)) {
            continue;
        }

        int distance = *distanceAt(next);

        if (distance >= 0 && distance < bestDistance) {
            bestDistance = distance;
            bestNext = next;
            bestDirection = directions[i];
        }
    }

    if (bestDistance != INT_MAX) {
        ghost->direction = bestDirection;
        setGhostCenter(ghost, bestNext);
    }
}

static void stepGhost(Ghost *ghost, float frightenedTimeLeft)
{
    switch (ghost->mode) {
        case GHOST_EXITING_HOUSE:
            releaseGhost(ghost, frightenedTimeLeft);
            break;
        case GHOST_EYES_RETURNING:
            stepReturning(ghost);
            break;
        case GHOST_NORMAL:
        case GHOST_FRIGHTENED:
            stepRoaming(ghost);
            break;
        case GHOST_REPLENISHING:
            break;
    }
}

void updateGhost(Ghost *ghost, float deltaTime, float frightenedTimeLeft)
{
    ghost->animationTimer += deltaTime;

    if (ghost->mode == GHOST_REPLENISHING) {
        ghost->replenishTimer -= deltaTime;

        if (ghost->replenishTimer <= 0.0f) {
            ghost->replenishTimer = 0.0f;
            ghost->movementAccumulator = 0.0f;
            ghost->mode = GHOST_EXITING_HOUSE;
        }

        return;
    }

    float speed = GHOST_NORMAL_SPEED;

    if (ghost->mode == GHOST_FRIGHTENED) {
        speed = GHOST_FRIGHTENED_SPEED;
    } else if (ghost->mode == GHOST_EYES_RETURNING) {
        speed = GHOST_EYES_SPEED;
    }

    ghost->movementAccumulator += deltaTime * speed;

    while (ghost->movementAccumulator >= 1.0f) {
        ghost->movementAccumulator -= 1.0f;
        stepGhost(ghost, frightenedTimeLeft);

        if (ghost->mode == GHOST_REPLENISHING) {
            break;
        }
    }
}

void drawGhost(const Ghost *ghost, float frightenedTimeLeft)
{
    int phase = (int)(ghost->animationTimer / 0.2f) % 2;
    int directionFrame;

    switch (ghost->direction) {
        case MAZE_RIGHT: directionFrame = 0; break;
        case MAZE_DOWN:  directionFrame = 2; break;
        case MAZE_LEFT:  directionFrame = 4; break;
        case MAZE_UP:    directionFrame = 6; break;
        default:         directionFrame = 0; break;
    }

    int frame;

    if (ghost->mode == GHOST_EYES_RETURNING) {
        frame = 12 + directionFrame / 2;
    } else if (ghost->mode == GHOST_FRIGHTENED) {
        frame = frightenedTimeLeft <= GHOST_FLASH_SECONDS
            ? 10 + phase
            : 8 + phase;
    } else {
        frame = directionFrame + phase;
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
