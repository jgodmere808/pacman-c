#pragma once

#include "config.h"

typedef struct {
    Texture2D background;
    Texture2D pacman;
    Texture2D dotSmall;
    Texture2D dotLarge;
    Texture2D blinky;
    Texture2D pinky;
    Texture2D inky;
    Texture2D clyde;
} TextureMap;

TextureMap textureMap;

void initTextureMap();
