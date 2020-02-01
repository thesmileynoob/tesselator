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
    const Uint8* Keys = SDL_GetKeyboardState(NULL);  // TODO?

    // slow motion!
    // hold LSHIFT to slow down player. Useful for some "powerdown"
    {  // effect_slowdown_player()

        vec2_reset(&Vel);

        const int is_slowmotion = Keys[SDL_SCANCODE_LSHIFT];
        const float factor      = is_slowmotion ? .5 : 1;
        vec2_scale(&Vel, factor);
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


    // player-level collision detection
    if (LEFT(this) < 0) { X = 0; }
    if (RIGHT(this) > SCR_WIDTH) { X = SCR_WIDTH - W; }
}

extern SDL_Renderer* _renderer;  // TODO
extern SDL_Texture* Texture;     // TODO
void player::Draw()
{
    SDL_Rect player_rect = RECT(this);
    SDL_Rect tex_rect    = texture_rect(1, 1);  // TODO
    SDL_RenderCopy(_renderer, Texture, &tex_rect, &player_rect);
}
