#pragma once

#include "ball.h"
#include "tile.h"

struct level {
    int Rows, Cols;
    int TileCount;
    tile* Tiles;

    player* Player;
    ball* Ball;
    SDL_Color BgCol;


    /// playable dimensions in abs px
    int Left;
    int Right;
    int Top;
    int Bottom;
    int Width;
    int Height;

public:
    void update();
    void draw();
};
