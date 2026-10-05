#pragma once

#include "config.h"
#include "maze.h"
#include "texture_map.h"

typedef enum {
    GHOST_BLINKY,
    GHOST_PINKY,
    GHOST_INKY,
    GHOST_CLYDE
} GhostName;

typedef enum {
    GHOST_NORMAL,
    GHOST_FRIGHTENED,
    GHOST_EYES_RETURNING,
    GHOST_REPLENISHING,
    GHOST_EXITING_HOUSE
} GhostMode;

typedef struct {
    GhostName name;
    Texture2D texture;
    GhostMode mode;
    float animationTimer;
    float movementAccumulator;
    float replenishTimer;
    Vector2 pos;
    MazeDirection direction;
} Ghost;

Ghost initGhost(GhostName name, Vector2 pos);
void ghostStartFrightened(Ghost *ghost);
void ghostEndFrightened(Ghost *ghost);
void ghostStartReturning(Ghost *ghost);
void updateGhost(Ghost *ghost, float deltaTime, float frightenedTimeLeft);
void drawGhost(const Ghost *ghost, float frightenedTimeLeft);
