#include "ball.h"
#include "breakout.h"
#include "player.h"

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

#if 0
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
    SDL_Rect player_rect = RECT(this);
    SDL_Rect tex_rect    = texture_rect(1, 1);  // TODO
    SDL_RenderCopy(_renderer, Texture, &tex_rect, &player_rect);
}


// TODO
int player::Animate()
{
    // player expand animation
    if (!IsAnimating || !Anim.frame_count) {
        return 0;  // TODO
    }

    const int amt = 5;
    if (Anim.frame_count <= 0) {
        // reset
        assert(IsAnimating == 1);
        IsAnimating      = 0;
        Anim.frame_count = 0;
        return 0;
    } else {
        // run animation
        Anim.frame_count--;

        // printf("animating: %s\n", __func__);
        X -= amt;
        W += 2 * amt;
        return 1;
    }
}

#endif
