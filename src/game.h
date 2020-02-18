#pragma once

#include <vector>

#include "animation.h"
#include "ball.h"
#include "level.h"
#include "particles.h"
#include "player.h"
#include "tile.h"

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

// meta
extern int Running;        // game running flag
extern int Score;          // current level score. you win when Score == TileCount
extern unsigned int Time;  // time taken to finish the level
extern level* Level;

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
void load_level(int n, int scrWidth, int scrHeight);
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
