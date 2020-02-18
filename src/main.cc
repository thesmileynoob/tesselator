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


/* TASK:
    - make fullscreen
    - define max grid
    - center game area

    - define level format
*/


void debug_break_all_tiles()
{
    for (int i = 0; i < game::Level->TileCount; ++i) {
        tile* t = &game::Level->Tiles[i];
        if (t->Hit) continue;
        game::event_tile_got_hit(t);
    }
}

/** here we go! */
int main(int argc, char const* argv[])
{
    // INIT
    gfx::init();
    // game must be init after gfx::init()
    game::load_level(0, gfx::_SCR_WIDTH, gfx::_SCR_HEIGHT);


    // main game loop
    game::Time       = 0;
    game::Score      = 0;
    game::debug_mode = 0;
    // game::Score   = game::TileCount - 5;  // TEMP: DEBUG: TESTING game end condition
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

                if (sym == SDLK_r) {
                    // TODO: for debugging only
                    debug_break_all_tiles();
                }
                if (sym == SDLK_p) { game::event_player_lost_life(); }
            }
        }

        // handle input
        {
            const Uint8* Keys    = SDL_GetKeyboardState(NULL);
            game::is_slow_motion = Keys[SDL_SCANCODE_LSHIFT] ? true : false;
        }

        // animation
        /// BUG: updating animations after Player or Ball causes an error
        game::update_animations(DT);

        // update state
        {
            game::Level->update();

            for (std::size_t i = 0; i < game::PSources.size(); ++i) {
                particle_src* psrc = game::PSources[i];
                if (psrc->IsDone()) { continue; }
                psrc->Update();
            }
        }


        // render
        game::draw_frame();

        // check win/lose condition
        if (game::is_game_over()) { game::event_game_over(game::GAME_OVER_WIN); }

        SDL_Delay(1000 / 60);  // fps
        game::Time += DT;
    }

    gfx::deinit();
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
