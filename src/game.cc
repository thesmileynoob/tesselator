#include "game.h"

#include "breakout.h"

namespace game
{

// todo move to main.cc
int level_left   = FRAME_WIDTH;
int level_right  = SCR_WIDTH - FRAME_WIDTH;
int level_top    = FRAME_WIDTH;
int level_bottom = SCR_HEIGHT - FRAME_WIDTH;

int Cols      = 5;
int Rows      = 3;
int TileCount = 5 * 3;

int GameRunning;    // game running flag
int Score;          // current level score. you win when Score == TileCount
unsigned int Time;  // time taken to finish the level

tile* Tiles    = nullptr;
player* Player = nullptr;
ball* Ball     = nullptr;
Uint8 BgCol[3];  // r,g,b


int is_game_over()
{
    if (game::Score < game::TileCount)
        return 0;  // not won
    else
        return 1;
}

}  // namespace game
