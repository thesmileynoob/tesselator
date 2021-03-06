#pragma once

#include <vector>

#include "ball.h"
#include "particles.h"
#include "player.h"
#include "tile.h"
#include "animation.h"

// structs and enums
namespace game
{
enum game_over_reason {
    GAME_OVER_WIN = 0,  // standard win condition
    GAME_OVER_DEAD,     // player dead
    GAME_OVER_TIMEUP,   // TODO
};
}

// variables
namespace game
{
extern bool debug_mode;

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

// meta
extern int Running;        // game running flag
extern int Score;          // current level score. you win when Score == TileCount
extern unsigned int Time;  // time taken to finish the level

// objects
extern tile* Tiles;
extern player* Player;
extern ball* Ball;
extern SDL_Color BgCol;

// particles
extern std::vector<particle_src*> PSources;

// effects
extern bool is_slow_motion;
extern float slow_motion_factor;

}  // namespace game


// functions
namespace game
{

// load level and init resources
void load_level(int n);
void reset_level();

bool is_game_over();
void draw_frame();

// animations
void queue_animation(animation_tag tag, unsigned int duration);
void update_animations(unsigned int DT);

// events
void event_tile_got_hit(tile* t);
void event_player_got_hit();
void event_player_lost_life();
void event_game_over(enum game_over_reason reason);

}  // namespace game
