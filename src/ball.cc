#include "ball.h"
#include "breakout.h"
#include "player.h"
#include "game.h"

#include <cassert>

extern player* Player;  // TODO
ball::ball()
    : object()  // call parent constructor
{
    X = Player->X + 40;  // TODO: 40
    Y = Player->Y - 50;
    W = 25;
    H = 25;

    const int BallXspeed = 5;
    const int BallYspeed = 14;

    Vel = vec2(BallXspeed, BallYspeed);
}

void ball::Draw()
{
    object::Draw();
    SDL_Rect ball_rect = RECT(this);
    SDL_SetRenderDrawColor(gfx::_renderer, 25, 25, 255, 255);
    SDL_RenderFillRect(gfx::_renderer, &ball_rect);
}

void ball::Update()
{
    // ball update
    X += Vel.X;
    Y += Vel.Y;


    // ball-level collision
    if (IsInside()) {
        // skip collision check and return
        return;
    }

    // resolve collision
    // printf("resolving ball-level collision\n");

    // resolve horizontal collision
    const int left_lim   = game::level_left;
    const int right_lim  = game::level_right;
    const int top_lim    = game::level_top;
    const int bottom_lim = game::level_bottom;

    if (LEFT(this) < left_lim) {
        X     = left_lim;
        Vel.X = -Vel.X;
    } else if (RIGHT(this) > right_lim) {
        X     = game::level_right - W;
        Vel.X = -Vel.X;
    }

    // resolve vertical collision
    if (TOP(this) < top_lim) {
        Y     = top_lim;
        Vel.Y = -Vel.Y;
    } else if (BOTTOM(this) > game::level_bottom) {
        Y     = game::level_bottom - H;
        Vel.Y = -Vel.Y;
    }
}
