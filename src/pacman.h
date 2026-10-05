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
    float animationTimer;
    Vector2 pos;
    Vector2 vel;
    Vector2 nextVel;
} Pacman;

Pacman initPacman(Vector2 pos);
void updatePacman(Pacman *pacman);
void drawPacman(Pacman *pacman);