#include <assert.h>
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
// slightly buggy af


/** here we go! */
int main(int argc, char const* argv[])
{
    // INIT
    gfx::init(SCR_WIDTH, SCR_HEIGHT);
    // game must be init after gfx::init()
    game::load_level(0);

    // main game loop
    game::Time    = 0;
    game::Running = 1;
    while (game::Running) {
        // handle system events
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            SDL_Keycode sym = ev.key.keysym.sym;
            if (sym == SDLK_q || ev.type == SDL_QUIT) { game::Running = 0; }

            if (ev.type == SDL_KEYUP) {
                if (sym == SDLK_RETURN) { game::Rows++; }
            } else if (ev.type == SDL_KEYDOWN) {
            }
        }

        // handle input and update state
        update_state(SDL_GetKeyboardState(NULL));

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
        game::Time += get_dt();
    }


    return 0;
}


// EVENT
// do a bunch of stuff when a tile gets hit
void on_tile_got_hit(tile* t)
{
    t->Hit++;       // mark it "Hit"
    t->Hidden = 1;  // don't draw hit tiles
    game::Score++;  // inc the score

    // create a new particle_src at the center of the hit tile
    int px             = CENTER_X(t);
    int py             = CENTER_Y(t);
    int count          = 8;
    particle_src* psrc = new particle_src(px, py, count, t->TexRow, t->TexCol);
    game::PSources.emplace_back(psrc);
}

void update_state(const Uint8* Keys)
{
    game::Player->Update();
    game::Ball->Update();

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
