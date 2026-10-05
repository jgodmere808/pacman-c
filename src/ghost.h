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
    GHOST_ANIMATION_NORMAL,
    GHOST_ANIMATION_EYES,
    GHOST_ANIMATION_BLINKING
} GhostAnimationState;

typedef struct {
    GhostName name;
    Texture2D texture;
    GhostAnimationState animationState;
    float animationTimer;
    Vector2 pos;
    MazeDirection direction;
    MazeDirection requestedDirection;
} Ghost;

Ghost initGhost(GhostName name, Vector2 pos);
void updateGhost(Ghost *ghost);
void drawGhost(Ghost *ghost);