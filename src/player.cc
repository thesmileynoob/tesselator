#include "player.h"
#include "breakout.h"
#include "game.h"
#include "gfx.h"

#include <cassert>

player::player()
    : object()  // call parent constructor
{
    const int lvl_center    = game::level_width / 2;
    const int player_width  = 155;
    const int player_height = 35;

    X = lvl_center - (player_width / 2);
    Y = 4.2 / 5.0 * game::level_height;
    W = player_width;
    H = player_height;

    Vel = vec2(15, 0);
}

void player::Update()
{
    const Uint8* Keys = SDL_GetKeyboardState(NULL);  // TODO?

    // slow motion!
    // hold LSHIFT to slow down player. Useful for some "powerdown"
    {                 // effect_slowdown_player()
        Vel.Reset();  // make sure we have the original velocity

        const int is_slowmotion = Keys[SDL_SCANCODE_LSHIFT];
        const float factor      = is_slowmotion ? .5 : 1;
        Vel.Scale(factor);  // decrease velocity
    }


    const int playerspeed = Vel.X;

    // movement
    if (Keys[SDL_SCANCODE_A]) {
        X -= playerspeed;  // move left
    }
    if (Keys[SDL_SCANCODE_D]) {
        X += playerspeed;  // move right
    }

    // actions
    if (Keys[SDL_SCANCODE_SPACE]) { puts("launch ball!"); }


    // resolve player-level collision
    const int left_lim  = game::level_left;
    const int right_lim = game::level_right;

    if (LEFT(this) < left_lim) { X = left_lim; }
    if (RIGHT(this) > right_lim) { X = right_lim - W; }
}


void player::Draw()
{
    object::Draw();

    SDL_Rect player_rect = RECT(this);
    SDL_Rect tex_rect    = gfx::texture_rect(1, 1);  // TODO
    gfx::draw_texture(gfx::Texture, &tex_rect, &player_rect);
    // SDL_RenderCopy(gfx::_renderer, Texture, &tex_rect, &player_rect);
}
