#pragma once

#include "config.h"
#include "texture_map.h"
#include "maze.h"

typedef enum {
    IDLE,
    FACING_UP,
    FACING_LEFT,
    FACING_RIGHT,
    FACING_DOWN,
    DIEING
} PacmanAnimationState;

typedef struct {
    Texture2D texture;
    PacmanAnimationState animationState;
    float animationTimer;
    Vector2 pos;
    MazeDirection direction;
    MazeDirection requestedDirection;
} Pacman;

Pacman initPacman(Vector2 pos);
void updatePacman(Pacman *pacman);
void startPacmanDeath(Pacman *pacman);
bool pacmanDeathFinished(const Pacman *pacman);
void drawPacman(const Pacman *pacman);
