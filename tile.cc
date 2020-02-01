#include "tile.h"
#include "breakout.h"

#include <cassert>

tile::tile()
    : object()  // call parent constructor
{

    // X, Y must be set manually
    // TexRow, TexCol must be set manually

    W = TILE_WIDTH;
    H = TILE_HEIGHT;

    Anim.frame_count = 5;
}

extern SDL_Renderer* _renderer;  // TODO
extern SDL_Texture* Texture;     // TODO
void tile::Draw()
{
    const int pad     = 12;
    const int x       = pad + X;
    const int y       = pad + Y;
    const int w       = W;
    const int h       = H;
    SDL_Rect tex_rect = texture_rect(TexRow, TexCol);

    SDL_Rect tile_rect = {x, y, w - 2 * pad, h - 2 * pad};
    SDL_RenderCopy(_renderer, Texture, &tex_rect, &tile_rect);
}

/** when a tile is hit
 * NOTE: This has to be sweeeeeet!
 * RETURNS: 0 on animation end
 */
int tile::Animate()
{
    if (!IsAnimating) {
        return 0;  // TODO
    }

    // anim_tile_breakout_animation(tile * t)
    // printf("animating: %s\n", __func__);
    const int gravity = 2;
    static int yspeed = 0;

    if (Anim.frame_count <= 0) {
        // reset
        // printf("RESET ANIM\n");
        // assert(IsAnimating == 1);
        IsAnimating      = 0;
        Anim.frame_count = 0;
        yspeed           = 0;
        return 0;
    } else {
        // run animation
        // printf("run \n");
        Anim.frame_count--;

        yspeed -= gravity;
        Y -= yspeed;
        return 1;
    }
}
#if 0
void ball::Update()
{
    // ball update
    X += Vel.X;
    Y += Vel.Y;


    // ball-level collision
    if (IsInside(0, SCR_WIDTH, 0, SCR_HEIGHT)) {
        // skip collision check
        return;
    } else {
        // resolve collision
        // printf("resolving ball-level collision\n");

        // resolve horizontal collision
        if (LEFT(this) < 0) {
            X     = 0;
            Vel.X = -Vel.X;
        } else if (RIGHT(this) > SCR_WIDTH) {
            X     = SCR_WIDTH - W;
            Vel.X = -Vel.X;
        }

        // resolve vertical collision
        if (TOP(this) < 0) {
            Y     = 0;
            Vel.Y = -Vel.Y;
        } else if (BOTTOM(this) > SCR_HEIGHT) {
            Y     = SCR_HEIGHT - H;
            Vel.Y = -Vel.Y;
        }
    }
}

#endif
