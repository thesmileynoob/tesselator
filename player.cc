#include "player.h"
#include "breakout.h"

#include <cassert>

player::player()
    : object()  // call parent constructor
{
    X = SCR_WIDTH / 2;
    Y = 4.2 / 5.0 * SCR_HEIGHT;
    W = 155;
    H = 35;

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


    // player-level collision detection
    if (LEFT(this) < 0) { X = 0; }
    if (RIGHT(this) > SCR_WIDTH) { X = SCR_WIDTH - W; }
}


extern SDL_Renderer* _renderer;  // TODO
extern SDL_Texture* Texture;     // TODO
void player::Draw()
{
    object::Draw();

    SDL_Rect player_rect = RECT(this);
    SDL_Rect tex_rect    = texture_rect(1, 1);  // TODO
    SDL_RenderCopy(_renderer, Texture, &tex_rect, &player_rect);
}
