#include "breakout.h"

void tile_breakout_animation(tile* t)
{
    puts("animating!");
    const int amt = 5;
    t->X -= amt;
    t->W += 2 * amt;
}
