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
static tile* get_nearest_tile();
static int effect_hl_nearest_tile();
static void reset_particle(object* p);


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

    // spawn particles
    game::ParticleSrcX = CENTER_X(t);
    game::ParticleSrcY = CENTER_Y(t);
    for (int i = 0; i < game::ParticleCount; ++i) {
        object* p = &game::Particles[i];
        game::reset_particle(p);
        // set particle texture matching the hit tile
        p->TexRow = t->TexRow;
        p->TexCol = t->TexCol;
    }
}

void update_state(const Uint8* Keys)
{
    game::Player->Update();

    game::Ball->Update();

    // ball-tiles collision
    {
        SDL_Rect ball_rect = RECT(game::Ball);

        SDL_Rect tile_rect;
        for (int i = 0; i < game::TileCount; ++i) {
            tile* t = &game::Tiles[i];
            if (t->Hit) continue;

            tile_rect = RECT(t);

            // most critical part of the whole codebase
            if (SDL_HasIntersection(&ball_rect, &tile_rect) == SDL_TRUE) {
                // ball has hit the tile
                const int ball_center = ball_rect.x + (ball_rect.w / 2);
                const int tile_center = tile_rect.x + (tile_rect.w / 2);
                const int max_xspeed  = 7;
                // -ve means ball to the left
                const int offset = (ball_center - tile_center) / 2;

                float scale    = 0.2;  // TODO: Better name
                int new_xspeed = scale * offset;
                if (new_xspeed > max_xspeed) new_xspeed = max_xspeed;    // clip max
                if (new_xspeed < -max_xspeed) new_xspeed = -max_xspeed;  // clip max

                // update Ball
                game::Ball->Vel.X = new_xspeed;          // based on offset
                game::Ball->Vel.Y = -game::Ball->Vel.Y;  // just changes direction in Y

                // perform the procedure
                on_tile_got_hit(t);
                break;
            }
        }
    }

    // ball-player collision
    {
        SDL_Rect ball_rect   = RECT(game::Ball);
        SDL_Rect player_rect = RECT(game::Player);
        if (SDL_HasIntersection(&ball_rect, &player_rect) == SDL_TRUE) {

            const int ball_center   = ball_rect.x + (ball_rect.w / 2);
            const int player_center = player_rect.x + (player_rect.w / 2);
            const int max_xspeed    = 7;

            // -ve means ball to the left
            const int offset = (ball_center - player_center) / 2;

            float scale    = 0.2;  // TODO: Better name
            int new_xspeed = scale * offset;
            if (new_xspeed > max_xspeed) new_xspeed = max_xspeed;    // clip max
            if (new_xspeed < -max_xspeed) new_xspeed = -max_xspeed;  // clip max

            game::Ball->Vel.X = new_xspeed;          // based on offset
            game::Ball->Vel.Y = -game::Ball->Vel.Y;  // just changes direction in Y
        }
    }
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
