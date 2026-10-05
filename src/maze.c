
#include "maze.h"

typedef struct {
    int fixed;
    int start;
    int end;
} Corridor;

/* { y, starting x, ending x } */
static const Corridor horizontal[] = {
    {  36,  12, 100 }, {  36, 124, 212 },
    {  68,  12, 212 },
    {  92,  12,  52 }, {  92,  76, 100 },
    {  92, 124, 148 }, {  92, 172, 212 },
    { 116,  76, 148 },
    { 140,  -8,  76 }, { 140, 148, 232 }, /* Tunnel */
    { 164,  76, 148 },
    { 188,  12, 100 }, { 188, 124, 212 },
    { 212,  12,  28 }, { 212,  52, 172 },
    { 212, 196, 212 },
    { 236,  12,  52 }, { 236,  76, 100 },
    { 236, 124, 148 }, { 236, 172, 212 },
    { 260,  12, 212 }
};

/* { x, starting y, ending y } */
static const Corridor vertical[] = {
    {  12,  36,  92 }, {  12, 188, 212 },
    {  12, 236, 260 },
    {  28, 212, 236 },
    {  52,  36, 236 },
    {  76,  68,  92 }, {  76, 116, 188 },
    {  76, 212, 236 },
    { 100,  36,  68 }, { 100,  92, 116 },
    { 100, 188, 212 }, { 100, 236, 260 },
    { 124,  36,  68 }, { 124,  92, 116 },
    { 124, 188, 212 }, { 124, 236, 260 },
    { 148,  68,  92 }, { 148, 116, 188 },
    { 148, 212, 236 },
    { 172,  36, 236 },
    { 196, 212, 236 },
    { 212,  36,  92 }, { 212, 188, 212 },
    { 212, 236, 260 }
};

static bool onCorridor(MazePoint point)
{
    for (int i = 0;
         i < (int)(sizeof(horizontal) / sizeof(horizontal[0]));
         i++) {
        const Corridor *c = &horizontal[i];

        if (point.y == c->fixed &&
            point.x >= c->start &&
            point.x <= c->end) {
            return true;
        }
    }

    for (int i = 0;
         i < (int)(sizeof(vertical) / sizeof(vertical[0]));
         i++) {
        const Corridor *c = &vertical[i];

        if (point.x == c->fixed &&
            point.y >= c->start &&
            point.y <= c->end) {
            return true;
        }
    }

    return false;
}

bool mazeTryStep(
    MazePoint center,
    MazeDirection direction,
    MazePoint *result
)
{
    if (result == NULL) {
        return false;
    }

    MazePoint next = center;

    switch (direction) {
        case MAZE_LEFT:  next.x--; break;
        case MAZE_RIGHT: next.x++; break;
        case MAZE_UP:    next.y--; break;
        case MAZE_DOWN:  next.y++; break;
        default: return false;
    }

    /*
     * The sprite is fully outside the 224-pixel image when its
     * center is -8 or 232. The next step enters the opposite side.
     */
    if (center.y == 140) {
        if (center.x == -8 && direction == MAZE_LEFT) {
            next.x = 232;
        } else if (center.x == 232 &&
                   direction == MAZE_RIGHT) {
            next.x = -8;
        }
    }

    if (!onCorridor(next)) {
        return false;
    }

    *result = next;
    return true;
}