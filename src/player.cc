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

    const float factor = game::is_slow_motion ? game::slow_motion_factor : 1;

    const int playerspeed = Vel.X * factor;

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
    const int left_lim  = 0;
    const int right_lim = game::level_width;

    if (Left() <= left_lim) { X = left_lim; }
    if (Right() >= right_lim) { X = right_lim - W; }
}


void player::Draw()
{
    if (Hidden) return;
    object::Draw();

    SDL_Rect player_rect = AbsRect();                // absolute position
    SDL_Rect tex_rect    = gfx::texture_rect(1, 1);  // TODO
    gfx::draw_texture(gfx::Texture, &tex_rect, &player_rect);


    if (game::debug_mode) {
        SDL_SetRenderDrawColor(gfx::_renderer, 255, 255, 255, 255);
        const int height = H + 20;
        const int width  = W + 20;
        gfx::draw_crosshair(AbsCenter(), width, height);
    }
}


void player::LoseLife()
{
    extern animation Anim;
    Anim.ShouldRun = true;
}
