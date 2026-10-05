
#include "texture_map.h"

void initTextureMap()
{
    textureMap.background = LoadTexture("resources/textures/pacman-arcade-maze.png");
    textureMap.pacman = LoadTexture("resources/textures/pacman-arcade-sprites.png");
    textureMap.dotSmall = LoadTexture("resources/textures/pacman-arcade-dot-small.png");
    textureMap.dotLarge = LoadTexture("resources/textures/pacman-arcade-dot-large.png");
}
