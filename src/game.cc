#include "game.h"

#include "breakout.h"
#include "particles.h"
#include "ui.h"

// game global variables
namespace game
{
bool debug_mode = true;

// meta
int Running;        // game running flag
int Score;          // current level score. you win when Score == TileCount
unsigned int Time;  // time taken to finish the level

// objects
level* Level;

// particles
std::vector<particle_src*> PSources;

// effects
bool is_slow_motion      = false;
float slow_motion_factor = .5;

// animations
// use queue_animation() and update_animations() for interaction
std::vector<animation*> _Animations;

}  // namespace game


// functions
namespace game
{

static tile* get_nearest_tile();
static int effect_hl_nearest_tile();

tile* get_nearest_tile()
{
    const vec2 ball_center = Level->Ball->center();
    const int ballx        = ball_center.X;
    const int bally        = ball_center.Y;

    tile* nearest_tile    = NULL;
    int nearest_tile_id   = -1;
    int nearest_tile_dist = 10000;
    for (int i = 0; i < game::Level->TileCount; ++i) {
        tile* t = &game::Level->Tiles[i];
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
        // printf("%s: no nearest tile found!\n", __func__);
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

void load_level(int n, int scrWidth, int scrHeight)
{
    Level = new level;
    assert(Level);

    // Set level params
    {
        Level->Left   = FRAME_WIDTH;
        Level->Right  = scrWidth - FRAME_WIDTH;
        Level->Top    = FRAME_WIDTH;
        Level->Bottom = scrHeight - FRAME_WIDTH;
        Level->Width  = Level->Right - Level->Left;
        Level->Height = Level->Bottom - Level->Top;

        Level->Cols      = 5;
        Level->Rows      = 3;
        Level->TileCount = Level->Cols * Level->Rows;
    }


    // alloc
    Level->Player = new player();
    Level->Ball   = new ball();
    Level->Tiles  = new tile[Level->TileCount];
    assert(Level->Player && Level->Ball && Level->Tiles);

    // background bg
    Level->BgCol = {0, 0, 0, 255};

    // tiles
    {
        // layout
        int xoff, yoff;   // keep track of row and column
        xoff = yoff = 0;  // start at top-left corner
        for (int i = 0; i < Level->TileCount; ++i, xoff += TILE_WIDTH) {

            // let potential value of tile
            int potential_right = xoff + TILE_WIDTH;

            // see if the tile goes off the right edge of the level
            if (potential_right > Level->Width) {
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
            assert(final_right <= Level->Width);
            assert(final_top >= 0);
            assert(final_bottom <= Level->Height);

            tile* t = &Level->Tiles[i];
            t->X    = final_left;
            t->Y    = final_top;
        }

        // everything else
        for (int i = 0; i < Level->TileCount; ++i) {
            tile* t   = &Level->Tiles[i];
            t->TexRow = i % 5;
            t->TexCol = (i + 2) % 5;
        }
    }
}

bool is_game_over()
{
    if (game::Score < Level->TileCount)
        return false;  // not won
    else
        return true;
}

void draw_frame()
{

    // START DRAWING
    SDL_SetRenderDrawColor(gfx::_renderer, Level->BgCol.r, Level->BgCol.g, Level->BgCol.b,
                           Level->BgCol.a);
    SDL_RenderClear(gfx::_renderer);

    // BACKGROUND
    {
        // fill the playable area with tiled background image

        /// TODO: Move these!
        // #define BG_WIDTH 426
        // #define BG_HEIGHT 492
        const int bg_width  = gfx::BgTexture_width;
        const int bg_height = gfx::BgTexture_height;
        // #undef BG_WIDTH
        // #undef BG_HEIGHT

        // draw inside these bounds
        const int left_edge   = Level->Left;
        const int right_edge  = Level->Right;
        const int top_edge    = Level->Top;
        const int bottom_edge = Level->Bottom;

        SDL_Rect dst_rect, src_rect;
        int x, y;
        // start from the top left corner
        x = left_edge;
        y = top_edge;
        while (1) {
            if (x >= right_edge && y >= bottom_edge) {
                // done painting
                break;
            }
            if (x >= right_edge) {
                // goto next beginning of next row
                x = left_edge;
                y += bg_height;
            }

            // full tile source texture
            src_rect.x = 0;
            src_rect.y = 0;
            src_rect.w = bg_width;
            src_rect.h = bg_height;

            // where the texture will be drawn
            dst_rect.x = x;
            dst_rect.y = y;
            dst_rect.w = bg_width;
            dst_rect.h = bg_height;


            const int bg_right  = x + bg_width;
            const int bg_bottom = y + bg_height;

            if (bg_right > right_edge) {
                // clip to right edge
                const int excess = bg_right - right_edge;
                src_rect.w -= excess;
                dst_rect.w -= excess;
            }
            if (bg_bottom > bottom_edge) {
                // clip to bottom edge
                const int excess = bg_bottom - bottom_edge;
                src_rect.h -= excess;
                dst_rect.h -= excess;
            }

            // draw background tile
            SDL_RenderCopy(gfx::_renderer, gfx::BgTexture, &src_rect, &dst_rect);

            x += bg_width;
        }
    }

    // GRID
    {
        if (game::debug_mode == true) {
            SDL_SetRenderDrawColor(gfx::_renderer, Level->BgCol.r + 150, Level->BgCol.g,
                                   Level->BgCol.b + 50, Level->BgCol.a);

            int x1, y1, x2, y2;

            // vertical lines
            for (int i = 0; i < Level->Cols + 1; ++i) {
                const int xoff = i * TILE_WIDTH;

                // x is constant
                x1 = x2 = Level->Left + xoff;

                y1 = Level->Top;
                y2 = y1 + gfx::_SCR_HEIGHT;

                SDL_RenderDrawLine(gfx::_renderer, x1, y1, x2, y2);
            }
            // horizontal lines
            for (int i = 0; i < Level->Rows + 1; ++i) {
                const int yoff = i * TILE_HEIGHT;

                // y is constant
                y1 = y2 = Level->Top + yoff;

                x1 = Level->Left;
                x2 = Level->Right;

                SDL_RenderDrawLine(gfx::_renderer, x1, y1, x2, y2);
            }
        }
    }

    // LEVEL
    {
        Level->draw();
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
        const int swidth  = (int) gfx::_SCR_WIDTH;
        const int sheight = (int) gfx::_SCR_HEIGHT;


        // upper wall
        SDL_Rect top_rect    = {0, 0, swidth, fw};
        SDL_Rect bottom_rect = {0, sheight - fw, swidth, fw};
        SDL_Rect left_rect   = {0, fw, fw, sheight - 2 * fw};
        SDL_Rect right_rect  = {swidth - fw, fw, fw, sheight - 2 * fw};

        SDL_SetRenderDrawColor(gfx::_renderer, 255, 0, 255, 50);

        SDL_RenderDrawRect(gfx::_renderer, &top_rect);
        SDL_RenderDrawRect(gfx::_renderer, &bottom_rect);
        SDL_RenderDrawRect(gfx::_renderer, &left_rect);
        SDL_RenderDrawRect(gfx::_renderer, &right_rect);

        // fill top rect
        SDL_SetRenderDrawColor(gfx::_renderer, 40, 120, 40, 255);
        SDL_RenderFillRect(gfx::_renderer, &top_rect);

        if (is_slow_motion) {
            // fill left, right and bottom rects with a nice color
            SDL_SetRenderDrawColor(gfx::_renderer, 0, 51, 153, 150);
            SDL_RenderFillRect(gfx::_renderer, &left_rect);
            SDL_RenderFillRect(gfx::_renderer, &right_rect);
            SDL_RenderFillRect(gfx::_renderer, &bottom_rect);
        }
    }

    // UI
    {  // score


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
        if (Level->Player->Dead) {
            SDL_Texture* texture;
            SDL_Rect rect;

            ui::gen_player_lose_life(&texture, &rect);

            // increase text size
            rect = scale_sdl_rect(rect, 1.3f);

            gfx::draw_texture(texture, NULL, &rect);
        }
    }

    // you win
    {
        bool condition = is_game_over();
        if (condition) {
            SDL_Texture* texture;
            SDL_Rect rect;

            ui::gen_generic_text("YOU WIN", &texture, &rect);
            rect.x = (gfx::_SCR_WIDTH / 2) - (rect.w / 2);
            rect.y = gfx::_SCR_HEIGHT / 2;

            // increase text size
            rect = scale_sdl_rect(rect, 1.3f);

            gfx::draw_texture(texture, NULL, &rect);
        }
    }

    SDL_RenderPresent(gfx::_renderer);
    // END DRAWING
}  // namespace game

/////// ANIMATIONS ////////
void queue_animation(animation_tag tag, unsigned int duration)
{
    animation* anim = new animation(tag, duration);
    anim->ShouldRun = true;  // TODO: why does this even exist?
    // printf("queuing animation: %s\n", anim->get_name());
    _Animations.emplace_back(anim);
}

void update_animations(unsigned int DT)
{
    // tick animation
    for (std::size_t i = 0; i < _Animations.size(); ++i) {
        animation* a = _Animations[i];
        if (a->ShouldRun && (!a->is_done())) { a->tick(DT); }
    }

    // delete finished animations
    auto start = _Animations.begin();
    for (std::size_t i = 0; i < _Animations.size(); ++i) {
        animation* a = _Animations[i];
        if (!a->is_done()) continue;
        // animation done
        // remove it from the list
        // printf("removing animation: %s\n", a->get_name());
        _Animations.erase(start + i);  /// TODO: does this call delete?????
    }
}


/////// EVENTS ////////

// do a bunch of stuff when a tile gets hit
void event_tile_got_hit(tile* t)
{
    t->Hit++;       // mark it "Hit"
    t->Hidden = 1;  // don't draw hit tiles
    game::Score++;  // inc the score

    // Spawn particles
    {
        // create a new particle_src at the center of the hit tile
        const vec2 tile_center = t->center();
        int count              = 8;
        particle_src* psrc =
            new particle_src(tile_center.X, tile_center.Y, count, t->TexRow, t->TexCol);
        game::PSources.emplace_back(psrc);
    }

    {  // flash bg
        queue_animation(ANIM_BLINK_SCREEN, 100);
    }
}


void event_player_got_hit()
{
    // start animation
    queue_animation(ANIM_PLAYER_HIT, 200);
}


// do a bunch of stuff when the player loses a life
void event_player_lost_life()
{
    if (Level->Player->Dead) return;
    Level->Player->lose_life();

    // start animation
    queue_animation(ANIM_PLAYER_LOSE_LIFE, 3000);
    printf("PLAYER DEAD\n");
}


static bool _called_once = false;
void event_game_over(enum game_over_reason reason)
{
    if (_called_once) { return; }
    _called_once = true;

    puts("*********************************");
    puts("Game Over");

    // print header. depends on 'reason'
    {
        switch (reason) {
        case GAME_OVER_WIN: puts("YOU WIN!"); break;
        case GAME_OVER_DEAD: puts("No lives left, YOU LOSE!"); break;
        default: puts("ERROR: GAME OVER NO REASON!"); break;
        }
    }

    // print common footer
    {
        // score
        printf("Score: %d\n", game::Score);

        // time taken
        const int in_secs = game::Time / 1000;
        const int mins    = in_secs / 60;
        const int secs    = in_secs % 60;
        printf("Time: ");
        if (mins) { printf("%d mins and ", mins); }
        printf("%d secs!\n", secs);
    }

    puts("*********************************");

    queue_animation(ANIM_GAME_OVER_DELAY, 3000);
}

}  // namespace game
