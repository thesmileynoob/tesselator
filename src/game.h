#pragma once

#include <vector>

#include "ball.h"
#include "particles.h"
#include "player.h"
#include "tile.h"


namespace game
{

/// level (aka playable area) dimensions
extern int level_left;
extern int level_right;
extern int level_top;
extern int level_bottom;
extern int level_width;
extern int level_height;


extern int Cols;
extern int Rows;
extern int TileCount;

extern int Running;        // game running flag
extern int Score;          // current level score. you win when Score == TileCount
extern unsigned int Time;  // time taken to finish the level

extern tile* Tiles;
extern player* Player;
extern ball* Ball;
extern Uint8 BgCol[3];  // r,g,b

extern std::vector<particle_src*> PSources;

// load level and init resources
void load_level(int n);
void reset_level();

void draw_frame();

bool is_game_over();
void reset_particle(object* p);

}  // namespace game
