#include "ball.h"
#include "breakout.h"
#include "game.h"
#include "player.h"

#include <cassert>

ball::ball()
    : object()  // call parent constructor
{
    X = game::Level->Player->X + 40;  // TODO: 40
    Y = game::Level->Player->Y - 50;
    W = 25;
    H = 25;

    const int BallXspeed = 5;
    const int BallYspeed = 14;

    Vel = vec2(BallXspeed, BallYspeed);
}


// NOTE: Always use Abs* for Drawing
void ball::draw()
{
    object::draw();

    // ball color
    SDL_Color bc = {255, 255, 255, 255};

    // draw ball
    SDL_Rect ball_rect = abs_rect();
    SDL_SetRenderDrawColor(gfx::_renderer, bc.r, bc.g, bc.b, bc.a);
    SDL_RenderFillRect(gfx::_renderer, &ball_rect);

    // render a "trail" in slow motion. This is just a hack where the ball rect
    // is drawn behind the current position by extrapolating previous
    // ball position and rendering increasingly transparent ball rectangles
    if (game::is_slow_motion) {
        if (SDL_SetRenderDrawBlendMode(gfx::_renderer, SDL_BLENDMODE_BLEND) == 1) {
            puts("blendmode ERROR");
        }

        const int lag      = 1.3;  // px
        SDL_Rect ball_rect = abs_rect();

        // draw first rect slightly behind ball
        ball_rect.x -= lag * Vel.X;
        ball_rect.y -= lag * Vel.Y;
        SDL_SetRenderDrawColor(gfx::_renderer, bc.r, bc.g, bc.b, bc.a - 100);
        // SDL_SetRenderDrawColor(gfx::_renderer, 25, 25, 255, 100);
        SDL_RenderFillRect(gfx::_renderer, &ball_rect);

        // draw second rect slightly behind the first rect
        ball_rect.x -= lag * Vel.X;
        ball_rect.y -= lag * Vel.Y;
        SDL_SetRenderDrawColor(gfx::_renderer, bc.r, bc.g, bc.b, bc.a - 200);
        // SDL_SetRenderDrawColor(gfx::_renderer, 25, 25, 255, 25);
        SDL_RenderFillRect(gfx::_renderer, &ball_rect);

        // draw ball again
        ball_rect = abs_rect();
        SDL_SetRenderDrawColor(gfx::_renderer, bc.r, bc.g, bc.b, bc.a);
        SDL_RenderFillRect(gfx::_renderer, &ball_rect);

        SDL_SetRenderDrawBlendMode(gfx::_renderer, SDL_BLENDMODE_NONE);
    }


    if (game::debug_mode) {
        SDL_SetRenderDrawColor(gfx::_renderer, 255, 255, 255, 255);
        const int height = H + 20;
        const int width  = W + 20;
        gfx::draw_crosshair(abs_center(), width, height);
    }
}


void ball::update()
{
    // ball update

    const float factor = game::is_slow_motion ? game::slow_motion_factor : 1;
    X += Vel.X * factor;
    Y += Vel.Y * factor;


    // ball-boundary collision
    // if we are not inside playable area, we skip this check
    if (!is_within_world()) {
        // resolve horizontal collision
        const int left_lim   = 0;
        const int right_lim  = game::Level->Width;
        const int top_lim    = 0;
        const int bottom_lim = game::Level->Height;

        if (left() <= left_lim) {
            X     = left_lim;
            Vel.X = -Vel.X;
            return;
        } else if (right() >= right_lim) {
            X     = right_lim - W;
            Vel.X = -Vel.X;
            return;
        }

        // resolve vertical collision
        if (top() < top_lim) {
            // hit top rect
            Y     = top_lim;
            Vel.Y = -Vel.Y;
            return;
        } else if (bottom() > bottom_lim) {
            // hit bottom rect
            Y     = bottom_lim - H;
            Vel.Y = -Vel.Y;

            // finally some action
            game::event_player_lost_life();
            return;
        }
    }


    // ball-tiles collision
    {
        SDL_Rect ball_rect = rect();

        SDL_Rect tile_rect;
        for (int i = 0; i < game::Level->TileCount; ++i) {
            tile* t = &game::Level->Tiles[i];
            if (t->Hit) continue;

            tile_rect = t->rect();

            // most critical part of the whole codebase
            if (SDL_HasIntersection(&ball_rect, &tile_rect) == SDL_TRUE) {
                handle_collision(t);

                // notify that a tile got hit
                game::event_tile_got_hit(t);
                break;
            }
        }
    }

    // ball-player collision
    {
        SDL_Rect ball_rect   = rect();
        SDL_Rect player_rect = game::Level->Player->rect();

        if (SDL_HasIntersection(&ball_rect, &player_rect) == SDL_TRUE) {
            handle_collision(game::Level->Player);

            // notify that player got hit
            game::event_player_got_hit();
        }
    }
}

void ball::handle_collision(object* obj)
{
    const int max_xspeed   = 7;  // max ball speed in X dir
    const vec2 ball_center = center();
    const vec2 obj_center  = obj->center();

    // offset b/w center of ball and player
    const int offset = (ball_center.X - obj_center.X) / 2;
    // printf("offset: %d\n", offset);

    float scale    = 0.2;  // TODO: Better name
    int new_xspeed = scale * offset;
    if (new_xspeed > max_xspeed) new_xspeed = max_xspeed;    // clip max
    if (new_xspeed < -max_xspeed) new_xspeed = -max_xspeed;  // clip max

    Vel.X = new_xspeed;  // based on offset
    Vel.Y = -Vel.Y;      // just changes direction in Y
}
