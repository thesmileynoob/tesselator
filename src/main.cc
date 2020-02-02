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


// TODO: move to particles.h?
const int ParticleCount = 8;
object* Particles;
int ParticleSrcX = SCR_WIDTH / 2;
int ParticleSrcY = SCR_HEIGHT / 2;

/** LOCAL FUNCTIONS */
// slightly buggy af
static tile* get_nearest_tile();
static int effect_hl_nearest_tile();
static void reset_particle(object* p);


/** here we go! */
int main(int argc, char const* argv[])
{
    // INIT
    {
        gfx::init(SCR_WIDTH, SCR_HEIGHT);

        // alloc
        game::Player = new player();
        game::Ball   = new ball();
        game::Tiles  = new tile[game::TileCount];
        Particles    = (object*) calloc(ParticleCount, sizeof(object));
        assert(game::Player && game::Ball && game::Tiles && Particles);

        // init gamestate

        // tiles
        {
            // layout
            int xoff, yoff;                   // keep track of row and column
            for (int i = 0, xoff = yoff = 0;  // top-left corner
                 i < game::TileCount;         // break condition
                 ++i, xoff += TILE_WIDTH) {

                // LET: potential values
                int pot_left   = xoff;
                int pot_top    = yoff;
                int pot_right  = xoff + TILE_WIDTH;
                int pot_bottom = yoff + TILE_HEIGHT;

                // perform checks
                if (pot_right > game::level_right) {
                    // printf("pot_right: %d, lvl_rig: %d\n", pot_right,
                    // game::level_right);
                    /// go to start of next row
                    xoff = 0;
                    yoff += TILE_HEIGHT;
                }

                // recalculate potential values
                pot_left   = xoff;
                pot_top    = yoff;
                pot_right  = xoff + TILE_WIDTH;
                pot_bottom = yoff + TILE_HEIGHT;

                // printf("i: %d, xoff: %d, yoff:%d\n", i, xoff, yoff);
                // printf("i: %d, potential_bottom: %d, lev_bot:%d\n", i, pot_bottom,
                //        game::level_bottom);

                // validate potential values
                assert(pot_left >= 0);
                assert(pot_right <= game::level_right);
                assert(pot_top >= 0);
                assert(pot_bottom <= game::level_bottom);

                tile* t = &game::Tiles[i];
                t->X    = pot_left;
                t->Y    = pot_top;
            }

            // everything else
            for (int i = 0; i < game::TileCount; ++i) {
                tile* t   = &game::Tiles[i];
                t->TexRow = i % 5;
                t->TexCol = (i + 2) % 5;
            }
        }

        // particles
        {

            for (int i = 0; i < ParticleCount; ++i) {
                object* p = &Particles[i];
                reset_particle(p);
                p->Hidden = 1;  // initially hidden
            }
        }
    }


    game::Time        = 0;
    game::GameRunning = 1;
    while (game::GameRunning) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            SDL_Keycode sym = ev.key.keysym.sym;
            if (sym == SDLK_q || ev.type == SDL_QUIT) { game::GameRunning = 0; }

            if (ev.type == SDL_KEYUP) {
                if (sym == SDLK_RETURN) { game::Rows++; }
            } else if (ev.type == SDL_KEYDOWN) {
            }
        }

        // handle input and update state
        update_state(SDL_GetKeyboardState(NULL));


        // START DRAWING
        SDL_SetRenderDrawColor(gfx::_renderer, game::BgCol[0], game::BgCol[1],
                               game::BgCol[2], 255);
        SDL_RenderClear(gfx::_renderer);

        // GRID
        {
            SDL_SetRenderDrawColor(gfx::_renderer, game::BgCol[0] + 150,
                                   game::BgCol[1] + 120, game::BgCol[2] * 0, 255);

            int x1, y1, x2, y2;

            // vertical lines
            for (int i = 0; i < game::Cols + 1; ++i) {
                const int xoff = i * TILE_WIDTH;

                // x is constant
                x1 = x2 = game::level_left + xoff;

                y1 = game::level_top;
                y2 = y1 + SCR_HEIGHT;

                SDL_RenderDrawLine(gfx::_renderer, x1, y1, x2, y2);
            }
            // horizontal lines
            for (int i = 0; i < game::Rows + 1; ++i) {
                const int yoff = i * TILE_HEIGHT;

                // y is constant
                y1 = y2 = game::level_top + yoff;

                x1 = game::level_left;
                x2 = game::level_right;

                SDL_RenderDrawLine(gfx::_renderer, x1, y1, x2, y2);
            }
        }

        // LEVEL
        {
            // TILES
            SDL_SetRenderDrawColor(gfx::_renderer, 255, 0, 0, 255);
            for (int i = 0; i < game::TileCount; ++i) {
                tile* t = &game::Tiles[i];
                t->Draw();
            }
        }


        // PLAYER
        {
            game::Player->Draw();
        }

        // BALL
        {
            game::Ball->Draw();
        }

        // "EFFECTS"
        {
            effect_hl_nearest_tile();

            // particles
            {

                const int xmin = ParticleSrcX - 100;
                const int xmax = ParticleSrcX + 100;

                const int ymin = ParticleSrcY - 100;
                const int ymax = ParticleSrcY + 100;

                for (int i = 0; i < ParticleCount; ++i) {
                    object* p = &Particles[i];
                    if (p->Hidden) continue;

                    p->X += p->Vel.X;
                    p->Y += p->Vel.Y;

                    if (p->X > xmax || p->X < xmin || p->Y > ymax || p->Y < ymin) {
                        // particle out of bounds. Don't draw it.
                        p->Hidden = 1;
                    }


                    SDL_Rect tex_rect      = gfx::texture_rect(p->TexRow, p->TexCol);
                    SDL_Rect particle_rect = RECT(p);
                    gfx::draw_texture(gfx::Texture, &tex_rect, &particle_rect);
                    // SDL_RenderCopy(gfx::_renderer, Texture, &tex_rect, &particle_rect);
                }
            }
        }

        // FRAME
        {
            // draw 4 thick filled rects. one for each side of the frame.
            const int fw      = FRAME_WIDTH;
            const int swidth  = (int) SCR_WIDTH;
            const int sheight = (int) SCR_HEIGHT;


            // upper wall
            SDL_Rect top_rect   = {0, 0, swidth, fw};
            SDL_Rect bot_rect   = {0, sheight - fw, swidth, fw};
            SDL_Rect left_rect  = {0, fw, fw, sheight - 2 * fw};
            SDL_Rect right_rect = {swidth - fw, fw, fw, sheight - 2 * fw};
            SDL_RenderDrawRect(gfx::_renderer, &top_rect);
            SDL_RenderDrawRect(gfx::_renderer, &bot_rect);
            SDL_RenderDrawRect(gfx::_renderer, &left_rect);
            SDL_RenderDrawRect(gfx::_renderer, &right_rect);

            // fill top rect
            SDL_SetRenderDrawColor(gfx::_renderer, 40, 120, 40, 255);
            SDL_RenderFillRect(gfx::_renderer, &top_rect);
        }

        // UI
        {
            // score
            {

                SDL_Texture* score_texture;
                SDL_Rect score_texture_rect;
                ui::gen_score(game::Score, &score_texture, &score_texture_rect);
                // SDL_RenderCopy(gfx::_renderer, score_texture, NULL,
                // &score_texture_rect);
                gfx::draw_texture(score_texture, NULL, &score_texture_rect);
            }

            // fps
            {
                static unsigned int old_time = 0;                    // ms
                unsigned int new_time        = SDL_GetTicks();       // ms
                unsigned int delta           = new_time - old_time;  // ms
                old_time                     = new_time;
                const int fps                = 1000 / delta;

                SDL_Texture* fps_texture;
                SDL_Rect fps_texture_rect;
                ui::gen_fps(fps, &fps_texture, &fps_texture_rect);
                gfx::draw_texture(fps_texture, NULL, &fps_texture_rect);
            }

            // time
            {
                SDL_Texture* time_texture;
                SDL_Rect time_texture_rect;
                ui::gen_time(game::Time, &time_texture, &time_texture_rect);
                gfx::draw_texture(time_texture, NULL, &time_texture_rect);
            }
        }


        SDL_RenderPresent(gfx::_renderer);
        // END DRAWING

        // check win/lose condition
        if (game::is_game_over()) {
            puts("Game Over");
            puts("You Win!");
            printf("Score: %d\n", game::Score);
            const int in_secs = game::Time / 1000;
            const int mins    = in_secs / 60;
            const int secs    = in_secs % 60;
            printf("Time: %d mins and %d secs!\n", mins, secs);
            game::GameRunning = 0;
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
    ParticleSrcX = CENTER_X(t);
    ParticleSrcY = CENTER_Y(t);
    for (int i = 0; i < ParticleCount; ++i) {
        object* p = &Particles[i];
        reset_particle(p);
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
                // printf("%d -> %d\n", BallXspeed, new_xspeed);
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

            // printf("%d -> %d\n", BallXspeed, new_xspeed);
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

tile* get_nearest_tile()
{
    const int ballx = CENTER_X(game::Ball);
    const int bally = CENTER_Y(game::Ball);

    tile* nearest_tile    = NULL;
    int nearest_tile_id   = -1;
    int nearest_tile_dist = 10000;
    for (int i = 0; i < game::TileCount; ++i) {
        tile* t = &game::Tiles[i];
        if (t->Hit) continue;  // skip hit tiles

        const int tilex = CENTER_X(t);
        const int tiley = CENTER_Y(t);

        // see you _do_ use the stuff you learn in school!
        const int xsqr = pow((ballx - tilex), 2);
        const int ysqr = pow((bally - tiley), 2);
        const int dist = sqrt(xsqr + ysqr);

        if (dist < nearest_tile_dist) {
            nearest_tile      = t;
            nearest_tile_id   = i;
            nearest_tile_dist = dist;
            // printf("nearest: id: %d, dist: %d\n", i, dist);
        }
    }

    if (nearest_tile == NULL) {
        printf("%s: no nearest tile found!\n", __func__);
        return NULL;
    }
    assert(nearest_tile_id != -1);
    assert(!nearest_tile->Hit);

    // printf("FINAL nearest: id: %d, dist: %d\n", nearest_tile_id, nearest_tile_dist);
    return nearest_tile;
}

int effect_hl_nearest_tile()
{

    tile* nearest_tile = get_nearest_tile();
    if (nearest_tile == NULL) { return 0; }

    SDL_Rect rect = RECT(nearest_tile);
    const int pad = 3;
    rect.x += game::level_left + pad;
    rect.y += game::level_top + pad;
    rect.w -= 2 * pad;
    rect.h -= 2 * pad;
    if (SDL_SetRenderDrawBlendMode(gfx::_renderer, SDL_BLENDMODE_BLEND) == 1) {
        puts("blendmode ERROR");
    }
    SDL_SetRenderDrawColor(gfx::_renderer, 255, 255, 255, 100);
    SDL_RenderFillRect(gfx::_renderer, &rect);
    SDL_SetRenderDrawBlendMode(gfx::_renderer, SDL_BLENDMODE_NONE);
    return 1;
}

void reset_particle(object* p)
{
    const int max_vy = 3;

    const int _rand_dir = rand() % 2 ? -1 : 1;  // random direction

    const int xoff = _rand_dir * (rand() % 13);
    const int yoff = _rand_dir * (rand() % 25);

    p->X = ParticleSrcX + xoff;
    p->Y = ParticleSrcY + yoff;
    p->W = (5) + rand() % 20;
    p->H = (5) + rand() % 20;


    p->Vel = (vec2){rand() % 2 ? -1 : 1, -1 * rand() % max_vy};


    p->TexRow = rand() % 5;
    p->TexCol = rand() % 5;

    p->Hidden = 0;
}
