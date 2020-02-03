#include "ball.h"
#include "breakout.h"
#include "game.h"
#include "player.h"

#include <cassert>

ball::ball()
    : object()  // call parent constructor
{
    X = game::Player->X + 40;  // TODO: 40
    Y = game::Player->Y - 50;
    W = 25;
    H = 25;

    const int BallXspeed = 5;
    const int BallYspeed = 14;

    Vel = vec2(BallXspeed, BallYspeed);
}


void ball::Draw()
{
    object::Draw();
    SDL_Rect ball_rect = AbsRect();
    SDL_SetRenderDrawColor(gfx::_renderer, 25, 25, 255, 255);
    SDL_RenderFillRect(gfx::_renderer, &ball_rect);
}


void ball::Update()
{
    // ball update
    const Uint8* Keys = SDL_GetKeyboardState(NULL);

    float factor = 1.;
    if (Keys[SDL_SCANCODE_LSHIFT]) { factor = .5; }
    X += Vel.X * factor;
    Y += Vel.Y * factor;


    // ball-boundary collision
    // if we are not inside playable area, we skip this check
    if (!IsInside()) {
        // resolve horizontal collision
        const int left_lim   = 0;
        const int right_lim  = game::level_width;
        const int top_lim    = 0;
        const int bottom_lim = game::level_height;

        if (Left() <= left_lim) {
            X     = left_lim;
            Vel.X = -Vel.X;
        } else if (Right() >= right_lim) {
            X     = right_lim - W;
            Vel.X = -Vel.X;
        }

        // resolve vertical collision
        if (Top() < top_lim) {
            Y     = top_lim;
            Vel.Y = -Vel.Y;
        } else if (Bottom() > bottom_lim) {
            Y     = bottom_lim - H;
            Vel.Y = -Vel.Y;
        }
    }


    // ball-tiles collision
    {
        SDL_Rect ball_rect = Rect();

        SDL_Rect tile_rect;
        for (int i = 0; i < game::TileCount; ++i) {
            tile* t = &game::Tiles[i];
            if (t->Hit) continue;

            tile_rect = t->Rect();

            // most critical part of the whole codebase
            if (SDL_HasIntersection(&ball_rect, &tile_rect) == SDL_TRUE) {
                // ball has hit the tile
                const int ball_center = ball_rect.x + (ball_rect.w / 2);
                const int tile_center = tile_rect.x + (tile_rect.w / 2);
                const int max_xspeed  = 7;
                // -ve means ball to the left
                const int offset = (ball_center - tile_center) / 2;

                float scale    = 0.2;  // TODO: Better name
                int new_xspeed = scale * offset;
                if (new_xspeed > max_xspeed) new_xspeed = max_xspeed;    // clip max
                if (new_xspeed < -max_xspeed) new_xspeed = -max_xspeed;  // clip max

                // update Ball
                game::Ball->Vel.X = new_xspeed;          // based on offset
                game::Ball->Vel.Y = -game::Ball->Vel.Y;  // just changes direction in Y

                // perform the procedure
                extern void on_tile_got_hit(tile * t);  /// TODO: TEMP
                on_tile_got_hit(t);
                break;
            }
        }
    }

    // ball-player collision
    {
        SDL_Rect ball_rect   = Rect();
        SDL_Rect player_rect = game::Player->Rect();

        if (SDL_HasIntersection(&ball_rect, &player_rect) == SDL_TRUE) {

            const int ball_center   = ball_rect.x + (ball_rect.w / 2);
            const int player_center = player_rect.x + (player_rect.w / 2);
            const int max_xspeed    = 7;

            // -ve means ball to the left
            const int offset = (ball_center - player_center) / 2;

            float scale    = 0.2;  // TODO: Better name
            int new_xspeed = scale * offset;
            if (new_xspeed > max_xspeed) new_xspeed = max_xspeed;    // clip max
            if (new_xspeed < -max_xspeed) new_xspeed = -max_xspeed;  // clip max

            game::Ball->Vel.X = new_xspeed;          // based on offset
            game::Ball->Vel.Y = -game::Ball->Vel.Y;  // just changes direction in Y
        }
    }
}
