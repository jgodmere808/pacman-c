#pragma once

#include "config.h"

typedef struct {
    Texture2D background;
    Texture2D pacman;
    Texture2D dotSmall;
    Texture2D dotLarge;
} TextureMap;

TextureMap textureMap;

void initTextureMap();
