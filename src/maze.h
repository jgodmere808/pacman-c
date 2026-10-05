#pragma once

#include "config.h"

typedef struct {
    int x;
    int y;
} MazePoint;

typedef enum {
    MAZE_LEFT,
    MAZE_RIGHT,
    MAZE_UP,
    MAZE_DOWN
} MazeDirection;

/*
 * Attempts to move one map pixel.
 * Returns false at a wall or corridor end.
 * On success, writes the new center to *result, including tunnel wrap.
 */
bool mazeTryStep(
    MazePoint center,
    MazeDirection direction,
    MazePoint *result
);