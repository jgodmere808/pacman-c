
#include "dots.h"
#include "texture_map.h"

#define DOT_TILE_SIZE 8
#define DOT_PICKUP_DISTANCE 3
#define LARGE_DOT_FLASH_SECONDS 0.20

/*
 * Each character represents one 8×8 map tile.
 * A tile at column c, row r has center (c * 8 + 4, r * 8 + 4).
 *
 * '.' = small dot
 * 'o' = large dot
 * ' ' = no dot
 */
static const char *const layout[DOT_ROWS] = {
    "                            ",
    "                            ",
    "                            ",
    "                            ",
    " ............  ............ ",
    " .    .     .  .     .    . ",
    " .    .     .  .     .    . ",
    " .    .     .  .     .    . ",
    " .......................... ",
    " .    .  .        .  .    . ",
    " .    .  .        .  .    . ",
    " o.....  .        .  .....o ",
    "      .              .      ",
    "      .              .      ",
    "      .  .        .  .      ",
    "      .  .        .  .      ",
    "      .  .        .  .      ",
    "                            ",
    "      .  .        .  .      ",
    "      .  .        .  .      ",
    "      .  ..........  .      ",
    "      .  .        .  .      ",
    "      .  .        .  .      ",
    " ............  ............ ",
    " .    .     .  .     .    . ",
    " .    .     .  .     .    . ",
    " ...  .......  .......  ... ",
    "   .  .  .        .  .  .   ",
    "   .  .  .        .  .  .   ",
    " o.....  ....  ....  .....o ",
    " .          .  .          . ",
    " .          .  .          . ",
    " .......................... ",
    "                            ",
    "                            ",
    "                            "
};

void initDots(Dots *dots)
{
    char symbol;
    int row, col;

    dots->remaining = 0;

    for (row = 0; row < DOT_ROWS; row++) {
        for (col = 0; col < DOT_COLS; col++) {
            symbol = layout[row][col];

            if (symbol == '.') {
                dots->cells[row][col] = DOT_SMALL;
                dots->remaining++;
            } else if (symbol == 'o') {
                dots->cells[row][col] = DOT_LARGE;
                dots->remaining++;
            } else {
                dots->cells[row][col] = DOT_NONE;
            }
        }
    }

    SetTextureFilter(textureMap.dotSmall, TEXTURE_FILTER_POINT);
    SetTextureFilter(textureMap.dotLarge, TEXTURE_FILTER_POINT);
}

DotType collectDot(Dots *dots, MazePoint pacmanCenter)
{
    DotType type;
    int row, col, dotX, dotY;

    for (row = 0; row < DOT_ROWS; row++) {
        for (col = 0; col < DOT_COLS; col++) {
            type = dots->cells[row][col];

            if (type == DOT_NONE) {
                continue;
            }

            dotX = col * DOT_TILE_SIZE + DOT_TILE_SIZE / 2;
            dotY = row * DOT_TILE_SIZE + DOT_TILE_SIZE / 2;

            if (abs(pacmanCenter.x - dotX) <= DOT_PICKUP_DISTANCE &&
                abs(pacmanCenter.y - dotY) <= DOT_PICKUP_DISTANCE) {
                dots->cells[row][col] = DOT_NONE;
                dots->remaining--;
                return type;
            }
        }
    }

    return DOT_NONE;
}

void drawDots(const Dots *dots)
{
    DotType type;
    int row, col;
    bool showLargeDots =
        ((int)(GetTime() / LARGE_DOT_FLASH_SECONDS) % 2) == 0;

    for (row = 0; row < DOT_ROWS; row++) {
        for (col = 0; col < DOT_COLS; col++) {
            type = dots->cells[row][col];

            if (type == DOT_NONE ||
                (type == DOT_LARGE && !showLargeDots)) {
                continue;
            }

            Texture2D texture =
                type == DOT_LARGE
                    ? textureMap.dotLarge
                    : textureMap.dotSmall;

            DrawTextureEx(
                texture,
                (Vector2){
                    col * DOT_TILE_SIZE * FACTOR,
                    row * DOT_TILE_SIZE * FACTOR
                },
                0.0f,
                FACTOR,
                WHITE
            );
        }
    }
}
