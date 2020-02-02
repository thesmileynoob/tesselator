#include "game.h"

#include "breakout.h"

namespace game
{

// todo move to main.cc
int level_left   = FRAME_WIDTH;
int level_right  = SCR_WIDTH - FRAME_WIDTH;
int level_top    = FRAME_WIDTH;
int level_bottom = SCR_HEIGHT - FRAME_WIDTH;

}  // namespace game
