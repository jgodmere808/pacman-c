
#include "texture_map.h"

void initTextureMap()
{
    textureMap.background = LoadTexture("resources/textures/pacman-arcade-maze.png");
    textureMap.pacman = LoadTexture("resources/textures/pacman-arcade-sprites.png");
    textureMap.dotSmall = LoadTexture("resources/textures/pacman-arcade-dot-small.png");
    textureMap.dotLarge = LoadTexture("resources/textures/pacman-arcade-dot-large.png");
    textureMap.blinky = LoadTexture("resources/textures/blinky.png");
    textureMap.pinky = LoadTexture("resources/textures/pinky.png");
    textureMap.inky = LoadTexture("resources/textures/inky.png");
    textureMap.clyde = LoadTexture("resources/textures/clyde.png");
}
