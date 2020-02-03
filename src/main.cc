#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_ttf.h>

#include "ball.h"
#include "breakout.h"
#include "game.h"
#include "gfx.h"
#include "player.h"
#include "tile.h"
#include "ui.h"


/** LOCAL FUNCTIONS */
animation Anim;

/** here we go! */
int main(int argc, char const* argv[])
{
    // INIT
    gfx::init(SCR_WIDTH, SCR_HEIGHT);
    // game must be init after gfx::init()
    game::load_level(0);

    Anim.Tag     = ANIM_PLAYER_LOSE_LIFE;
    Anim.Time    = 1000;
    Anim.Elapsed = 0;

    // main game loop
    game::Time    = 0;
    game::Running = 1;
    while (game::Running) {
        const unsigned int DT = get_dt();

        // handle system events
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            SDL_Keycode sym = ev.key.keysym.sym;
            if (sym == SDLK_q || ev.type == SDL_QUIT) { game::Running = 0; }

            if (ev.type == SDL_KEYUP) {
                if (sym == SDLK_RETURN) {
                    // toggle debug_mode
                    game::debug_mode = !game::debug_mode;
                }
                // change slow_motion_factor
                if (sym == SDLK_w) {
                    game::slow_motion_factor += .1;
                } else if (sym == SDLK_s) {
                    game::slow_motion_factor -= .1;
                }
                // reset slow_motion_factor
                if (sym == SDLK_r) { game::slow_motion_factor -= .5; }
            }
        }

        // handle input
        {
            const Uint8* Keys    = SDL_GetKeyboardState(NULL);
            game::is_slow_motion = Keys[SDL_SCANCODE_LSHIFT] ? true : false;
        }


        // update state
        game::Player->Update();
        game::Ball->Update();

        // animation
        if (!Anim.IsDone()) {
            printf("elaspsed: %4d, time: %d, DT: %d\n", Anim.Elapsed, Anim.Time, DT);
            Anim.Tick(DT);
        }

        // render
        game::draw_frame();

        // check win/lose condition
        if (game::is_game_over()) {
            puts("Game Over");
            puts("You Win!");
            printf("Score: %d\n", game::Score);
            const int in_secs = game::Time / 1000;
            const int mins    = in_secs / 60;
            const int secs    = in_secs % 60;
            printf("Time: %d mins and %d secs!\n", mins, secs);
            game::Running = 0;
            continue;
        }

        SDL_Delay(1000 / 60);  // fps
        game::Time += DT;
    }


    return 0;
}


/**
 * returns: dt in millisecs
 */
unsigned int get_dt()
{
    static unsigned int oldtime = 0;
    const unsigned int newtime  = SDL_GetTicks();
    const unsigned int dt       = newtime - oldtime;
    oldtime                     = newtime;
    return dt;
}
