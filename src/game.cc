#include "game.h"

#include "breakout.h"
#include "particles.h"
#include "ui.h"

namespace game
{

// todo move to main.cc
int level_left   = FRAME_WIDTH;
int level_right  = SCR_WIDTH - FRAME_WIDTH;
int level_top    = FRAME_WIDTH;
int level_bottom = SCR_HEIGHT - FRAME_WIDTH;

int Cols      = 5;
int Rows      = 3;
int TileCount = 5 * 3;

int Running;        // game running flag
int Score;          // current level score. you win when Score == TileCount
unsigned int Time;  // time taken to finish the level

tile* Tiles    = nullptr;
player* Player = nullptr;
ball* Ball     = nullptr;
Uint8 BgCol[3];  // r,g,b

std::vector<particle_src*> PSources;
// const int ParticleCount = 8;
// object* Particles;
// int ParticleSrcX = SCR_WIDTH / 2;
// int ParticleSrcY = SCR_HEIGHT / 2;


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
        }
    }

    if (nearest_tile == NULL) {
        printf("%s: no nearest tile found!\n", __func__);
        return NULL;
    }
    assert(nearest_tile_id != -1);
    assert(!nearest_tile->Hit);

    return nearest_tile;
}

int effect_hl_nearest_tile()
{

    tile* nearest_tile = get_nearest_tile();
    if (nearest_tile == NULL) { return 0; }

    const int pad = 5;
    SDL_Rect rect = RECT(nearest_tile);
    rect.x += game::level_left + pad;
    rect.y += game::level_top + pad;
    rect.w -= 2 * pad;
    rect.h -= 2 * pad;

    // draw a rect over the tile to simulate "highlighting"
    if (SDL_SetRenderDrawBlendMode(gfx::_renderer, SDL_BLENDMODE_BLEND) == 1) {
        puts("blendmode ERROR");
    }
    SDL_SetRenderDrawColor(gfx::_renderer, 0, 0, 255, 50);
    SDL_RenderFillRect(gfx::_renderer, &rect);
    SDL_SetRenderDrawBlendMode(gfx::_renderer, SDL_BLENDMODE_NONE);

    return 1;
}

void load_level(int n)
{
    // alloc
    Player = new player();
    Ball   = new ball();
    Tiles  = new tile[game::TileCount];
    // Particles = (object*) calloc(ParticleCount, sizeof(object));
    assert(Player && Ball && Tiles);

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
                /// go to start of next row
                xoff = 0;
                yoff += TILE_HEIGHT;
            }

            // recalculate potential values
            pot_left   = xoff;
            pot_top    = yoff;
            pot_right  = xoff + TILE_WIDTH;
            pot_bottom = yoff + TILE_HEIGHT;

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
}

bool is_game_over()
{
    if (game::Score < game::TileCount)
        return false;  // not won
    else
        return true;
}


void draw_frame()
{

    // START DRAWING
    SDL_SetRenderDrawColor(gfx::_renderer, game::BgCol[0], game::BgCol[1], game::BgCol[2],
                           255);
    SDL_RenderClear(gfx::_renderer);

    // GRID
    {
        SDL_SetRenderDrawColor(gfx::_renderer, game::BgCol[0] + 150, game::BgCol[1] + 120,
                               game::BgCol[2] * 0, 255);

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
        for (int i = 0; i < PSources.size(); ++i) {
            particle_src* psrc = PSources[i];
            if (psrc->IsDone()) { continue; }
            psrc->Draw();
        }

        // delete done particle sources
        for (int i = 0; i < PSources.size(); ++i) {
            particle_src* psrc = PSources[i];
            auto start         = PSources.begin();
            if (psrc->IsDone()) { PSources.erase(start + i); }
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

        SDL_SetRenderDrawColor(gfx::_renderer, 255, 0, 255, 50);

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
}

}  // namespace game
