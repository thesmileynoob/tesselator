#include "player.h"
#include "breakout.h"


player::player()
    : object()  // call parent constructor
{
    X = SCR_WIDTH / 2;
    Y = 4.2 / 5.0 * SCR_HEIGHT;
    W = 155;
    H = 35;

    Vel = vec2_create(15, 0);

    Anim.anim_func   = &anim_player_expand;  // press b to expand!
    Anim.frame_count = 5;
}

void player::Update()
{
    // TODO
}

void player::Draw()
{
    // TODO
}
