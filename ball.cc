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

extern SDL_Renderer* _renderer;  // TODO
extern SDL_Texture* Texture;     // TODO
void ball::Draw()
{
    SDL_Rect ball_rect = RECT(this);
    SDL_SetRenderDrawColor(_renderer, 25, 25, 255, 255);
    SDL_RenderFillRect(_renderer, &ball_rect);
}

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
