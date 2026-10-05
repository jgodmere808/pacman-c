
#include "pacman.h"

Pacman initPacman(Vector2 pos)
{
    Pacman pacman = {
        .texture = textureMap.pacman,
        .animationState = IDLE,
        .pos = pos,
        .vel = (Vector2){ 0, 0 }
    };

    return pacman;
}

void updatePacman(Pacman *pacman)
{
    return;
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
