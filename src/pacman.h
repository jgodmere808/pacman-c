#pragma once

#include "config.h"
#include "texture_map.h"

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
    Vector2 pos;
    Vector2 vel;
} Pacman;

Pacman initPacman(Vector2 pos);
void updatePacman(Pacman *pacman);
void drawPacman(Pacman *pacman);