#pragma once

#include "config.h"

typedef struct {
    Texture2D background;
    Texture2D pacman;
} TextureMap;

TextureMap textureMap;

void initTextureMap();
