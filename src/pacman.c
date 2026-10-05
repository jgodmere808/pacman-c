
#include "pacman.h"

#define PACMAN_SPEED 20.0f

Pacman initPacman(Vector2 pos)
{
    Pacman pacman = {
        .texture = textureMap.pacman,
        .animationState = IDLE,
        .pos = pos,
        .vel = (Vector2){ 1, 0 },
        .nextVel = (Vector2){ 1, 0 }
    };

    return pacman;
}

void updatePacman(Pacman *pacman)
{
    float nextX, nextY;

    if (IsKeyPressed(KEY_LEFT) && !IsKeyPressed(KEY_RIGHT)) {
        // left
        pacman->vel = (Vector2){ -1, 0 };
    }
    if (!IsKeyPressed(KEY_LEFT) && IsKeyPressed(KEY_RIGHT)) {
        // right
        pacman->vel = (Vector2){ 1, 0 };
    }
    if (IsKeyPressed(KEY_UP) && !IsKeyPressed(KEY_DOWN)) {
        // up
        pacman->vel = (Vector2){ 0, -1 };
    }
    if (!IsKeyPressed(KEY_UP) && IsKeyPressed(KEY_DOWN)) {
        // down
        pacman->vel = (Vector2){ 0, 1 };
    }

    nextX = pacman->vel.x + pacman->pos.x;
    nextY = pacman->vel.y + pacman->pos.y;

    // update pacman position
    pacman->pos = (Vector2){ nextX, nextY };
}

void drawPacman(Pacman *pacman)
{
    int frame;
    Rectangle source;

    switch (pacman->animationState) {
        case IDLE:
            frame = 4;
            break;
    }

    source = (Rectangle){ frame * 16, 0, 16, 16 };

    DrawTexturePro(
        pacman->texture,
        source,
        (Rectangle){
            pacman->pos.x * FACTOR,
            pacman->pos.y * FACTOR,
            16 * FACTOR,
            16 * FACTOR
        },
        (Vector2){ 0, 0 },
        0.0f,
        WHITE
    );
}
