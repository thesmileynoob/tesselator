#include "game.h"

#include "breakout.h"
#include "particles.h"
#include "ui.h"

static bool DRAW_OUCH = false;

// game global variables
namespace game
{
bool debug_mode = true;

/// level (aka playable area) dimensions
int level_left   = FRAME_WIDTH;
int level_right  = SCR_WIDTH - FRAME_WIDTH;
int level_top    = FRAME_WIDTH;
int level_bottom = SCR_HEIGHT - FRAME_WIDTH;
int level_width  = level_right - level_left;
int level_height = level_bottom - level_top;

int Cols      = 5;
int Rows      = 3;
int TileCount = 5 * 3;

// meta
int Running;        // game running flag
int Score;          // current level score. you win when Score == TileCount
unsigned int Time;  // time taken to finish the level

// objects
tile* Tiles    = nullptr;
player* Player = nullptr;
ball* Ball     = nullptr;
Uint8 BgCol[3];  // r,g,b

// particles
std::vector<particle_src*> PSources;

// effects
bool is_slow_motion      = false;
float slow_motion_factor = .5;

// animations
std::vector<animation*> Animations;

}  // namespace game


// functions
namespace game
{

static tile* get_nearest_tile();
static int effect_hl_nearest_tile();

tile* get_nearest_tile()
{
    const vec2 ball_center = Ball->center();
    const int ballx        = ball_center.X;
    const int bally        = ball_center.Y;

    tile* nearest_tile    = NULL;
    int nearest_tile_id   = -1;
    int nearest_tile_dist = 10000;
    for (int i = 0; i < game::TileCount; ++i) {
        tile* t = &game::Tiles[i];
        if (t->Hit) continue;  // skip hit tiles

        const vec2 tile_center = t->center();
        const int tilex        = tile_center.X;
        const int tiley        = tile_center.Y;

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

// NOTE: this is a DRAWING function. Use AbsRect()
int effect_hl_nearest_tile()
{

    tile* nearest_tile = get_nearest_tile();
    if (nearest_tile == NULL) { return 0; }

    const int pad = 5;
    SDL_Rect rect = nearest_tile->abs_rect();
    rect.x += pad;
    rect.y += pad;
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
    assert(Player && Ball && Tiles);

    // tiles
    {
        // layout
        int xoff, yoff;   // keep track of row and column
        xoff = yoff = 0;  // start at top-left corner
        for (int i = 0; i < game::TileCount; ++i, xoff += TILE_WIDTH) {

            // let potential value of tile
            int potential_right = xoff + TILE_WIDTH;

            // see if the tile goes off the right edge of the level
            if (potential_right > game::level_width) {
                /// go to beginning of next row
                xoff = 0;             // beginning
                yoff += TILE_HEIGHT;  // next row
            }

            // final values
            const int final_left   = xoff;
            const int final_top    = yoff;
            const int final_right  = xoff + TILE_WIDTH;
            const int final_bottom = yoff + TILE_HEIGHT;

            // validate potential values
            assert(final_left >= 0);
            assert(final_right <= game::level_width);
            assert(final_top >= 0);
            assert(final_bottom <= game::level_height);

            tile* t = &game::Tiles[i];
            t->X    = final_left;
            t->Y    = final_top;
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
            t->draw();
        }
    }


    // PLAYER
    {
        game::Player->draw();
    }

    // BALL
    {
        game::Ball->draw();
    }

    // "EFFECTS"
    {
        effect_hl_nearest_tile();
        for (std::size_t i = 0; i < PSources.size(); ++i) {
            particle_src* psrc = PSources[i];
            if (psrc->IsDone()) { continue; }
            psrc->Draw();
        }

        // delete done particle sources
        auto start = PSources.begin();
        for (std::size_t i = 0; i < PSources.size(); ++i) {
            particle_src* psrc = PSources[i];
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

            SDL_Texture* texture;
            SDL_Rect rect;
            ui::gen_score(game::Score, &texture, &rect);
            gfx::draw_texture(texture, NULL, &rect);
        }

        // fps
        {
            static unsigned int old_time = 0;                    // ms
            unsigned int new_time        = SDL_GetTicks();       // ms
            unsigned int delta           = new_time - old_time;  // ms
            old_time                     = new_time;
            const int fps                = 1000 / delta;

            SDL_Texture* texture;
            SDL_Rect rect;
            ui::gen_fps(fps, &texture, &rect);
            gfx::draw_texture(texture, NULL, &rect);
        }

        // time
        {
            SDL_Texture* texture;
            SDL_Rect rect;
            ui::gen_time(game::Time, &texture, &rect);
            gfx::draw_texture(texture, NULL, &rect);
        }


        // player_lose_life
        {
            if (DRAW_OUCH) {
                SDL_Texture* texture;
                SDL_Rect rect;

                ui::gen_player_lose_life(&texture, &rect);

                // increase text size
                rect = scale_sdl_rect(rect, 1.3f);

                gfx::draw_texture(texture, NULL, &rect);
            }
        }
    }


    SDL_RenderPresent(gfx::_renderer);
    // END DRAWING
}


/////// EVENTS ////////

// do a bunch of stuff when a tile gets hit
void on_tile_got_hit(tile* t)
{
    t->Hit++;       // mark it "Hit"
    t->Hidden = 1;  // don't draw hit tiles
    game::Score++;  // inc the score

    // create a new particle_src at the center of the hit tile
    const vec2 tile_center = t->center();
    int count              = 8;
    particle_src* psrc =
        new particle_src(tile_center.X, tile_center.Y, count, t->TexRow, t->TexCol);
    game::PSources.emplace_back(psrc);
}

void on_player_lose_life()
{
    if (DRAW_OUCH)
        return;  // do nothing for now // TODO: if(Player->life == 0) or something

    Player->lose_life();
    DRAW_OUCH = true;
    // slow_motion_factor = .1; /// TODO
    // is_slow_motion     = true;
    printf("GameOVER - YOU LOSE\n");
}

void on_game_over()
{
    // right now score == tilecount means WIN
    bool due_to_all_tile_broken = game::Score == game::TileCount;

    puts("*********************************");
    puts("Game Over");

    // print header
    {
        if (due_to_all_tile_broken) {
            // win due to all tiles broken
            puts("YOU WIN!");
        } else {
            // lose due to no player lives left
            puts("No lives left, YOU LOSE!");
        }
    }

    // print common footer
    {
        printf("Score: %d\n", game::Score);
        const int in_secs = game::Time / 1000;
        const int mins    = in_secs / 60;
        const int secs    = in_secs % 60;
        printf("Time: ");
        if (mins) { printf("%d mins and ", mins); }
        printf("%d secs!\n", secs);
    }

    puts("*********************************");

    // ultimate objective
    game::Running = false;
}


}  // namespace game
