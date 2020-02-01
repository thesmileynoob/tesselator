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

extern SDL_Renderer* _renderer;  // TODO
extern SDL_Texture* Texture;     // TODO
void player::Draw()
{
    SDL_Rect player_rect = RECT(this);
    SDL_Rect tex_rect    = texture_rect(1, 1);  // TODO
    SDL_RenderCopy(_renderer, Texture, &tex_rect, &player_rect);
}
