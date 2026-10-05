#pragma once

#include "config.h"
#include "maze.h"

#define DOT_ROWS 36
#define DOT_COLS 28

typedef enum {
    DOT_NONE,
    DOT_SMALL,
    DOT_LARGE
} DotType;

typedef struct {
    DotType cells[DOT_ROWS][DOT_COLS];
    int remaining;
} Dots;

void initDots(Dots *dots);
DotType collectDot(Dots *dots, MazePoint pacmanCenter);
void drawDots(const Dots *dots);