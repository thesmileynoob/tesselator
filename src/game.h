#pragma once
#include "ball.h"
#include "player.h"
#include "tile.h"


namespace game
{

extern int level_left;
extern int level_right;
extern int level_top;
extern int level_bottom;


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


// TODO: move to particles.h?
extern const int ParticleCount;
extern object* Particles;
extern int ParticleSrcX;
extern int ParticleSrcY;


// load level and init resources
void load_level(int n);
void reset_level();

void draw_frame();

bool is_game_over();
void reset_particle(object* p);

}  // namespace game
