#include <SDL2/SDL_keycode.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "breakout.h"


// globals
int GameRunning;    // game running flag
int Score;          // current level score. you win when Score == TileCount
unsigned int Time;  // time taken to finish the level

int Cols      = 5;
int Rows      = 6;
int TileCount = 5 * 6;
// int Cols      = 2;
// int Rows      = 2;
// int TileCount = 2 * 2;
tile* Tiles = NULL;

object* Player;

object* Ball;

SDL_Texture* Texture;
Uint8 BgCol[3];  // r,g,b

TTF_Font* UIFont;


// env
SDL_Window* _window     = NULL;
SDL_Renderer* _renderer = NULL;

// slightly buggy af
tile* get_nearest_tile()
{
    const int ballx = CENTER_X(Ball);
    const int bally = CENTER_Y(Ball);

    tile* nearest_tile    = NULL;
    int nearest_tile_id   = -1;
    int nearest_tile_dist = 10000;
    for (int i = 0; i < TileCount; ++i) {
        tile* t = &Tiles[i];
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
    const int pad = 8;
    rect.x += pad;
    rect.y += pad;
    rect.w -= 2 * pad;
    rect.h -= 2 * pad;
    if (SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND) == 1) {
        puts("blendmode ERROR");
    }
    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 100);
    SDL_RenderFillRect(_renderer, &rect);
    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_NONE);
    return 1;
}


// hold LSHIFT to slow down player. Useful for some "powerdown"
int effect_slowdown_player(const Uint8* Keys)
{
    vec2_reset(&Player->Vel);

    const int is_slowmotion = Keys[SDL_SCANCODE_LSHIFT];
    const float factor      = is_slowmotion ? .5 : 1;
    vec2_scale(&Player->Vel, factor);
    return 1;
}


/** here we go! */
int main(int argc, char const* argv[])
{
    // INIT
    {
        _init_sdl(SCR_WIDTH, SCR_HEIGHT, &_window, &_renderer);
        assert(_window && _renderer);

        Texture = load_texture("../assets/tiles.png");
        assert(Texture);

        UIFont = TTF_OpenFont("../assets/font_04b.ttf", 25);
        assert(UIFont);

        // alloc
        Player = calloc(1, sizeof(object));
        Ball   = calloc(1, sizeof(object));
        Tiles  = calloc(TileCount, sizeof(object));
        assert(Player && Ball && Tiles);

        // init gamestate

        // player
        Player->X = SCR_WIDTH / 2;
        Player->Y = 4.2 / 5.0 * SCR_HEIGHT;
        Player->W = 155;
        Player->H = 35;

        Player->Vel = vec2_create(15, 0);

        Player->Anim.anim_func   = &anim_player_expand;  // press b to expand!
        Player->Anim.frame_count = 5;

        // ball
        Ball->X              = Player->X + 40;  // TODO: 40
        Ball->Y              = Player->Y - 50;
        Ball->W              = 25;
        Ball->H              = 25;
        const int BallXspeed = 2;
        const int BallYspeed = 8;
        Ball->Vel            = vec2_create(BallXspeed, BallYspeed);

        // tiles
        int xoff, yoff;  // keep track of row and column
        xoff = yoff = 0;
        for (int i = 0; i < TileCount; ++i) {
            tile* t   = &Tiles[i];
            t->X      = xoff;
            t->Y      = yoff;
            t->W      = TILE_WIDTH;
            t->H      = TILE_HEIGHT;
            t->TexRow = i % 5;
            t->TexCol = (i + 2) % 5;

            t->Anim.anim_func   = &anim_tile_breakout_animation;
            t->Anim.frame_count = 5;


            xoff += TILE_WIDTH;
            if (RIGHT(t) > SCR_WIDTH) {
                // wrap back to first column and the next row
                xoff = 0;
                yoff += TILE_HEIGHT;
            }
        }
    }


    GameRunning = 1;
    while (GameRunning) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            SDL_Keycode sym = ev.key.keysym.sym;
            if (sym == SDLK_q || ev.type == SDL_QUIT) { GameRunning = 0; }

            if (ev.type == SDL_KEYUP) {
                if (sym == SDLK_RETURN) { Rows++; }
                if (sym == SDLK_b) {
                    Player->IsAnimating = 1;
                    if (!Player->Anim.frame_count) Player->Anim.frame_count += 5;
                    printf("frame_count: %d\n", Player->Anim.frame_count);
                }
            } else if (ev.type == SDL_KEYDOWN) {
            }
        }

        // handle input and update state
        update_state(SDL_GetKeyboardState(NULL));


        Time += get_dt();

        // START ANIMATING
        {
            animation* anim = &Player->Anim;
            if (Player->IsAnimating && anim->frame_count) {
                anim->frame_count--;
                Player->Anim.anim_func(Player);
            }


            for (int i = 0; i < TileCount; ++i) {
                tile* t = &Tiles[i];
                if (t->Hit && t->IsAnimating) {
                    t->Anim.frame_count--;
                    t->Anim.anim_func(t);
                }
            }
        }
        // END ANIMATING


        // START DRAWING
        SDL_SetRenderDrawColor(_renderer, BgCol[0], BgCol[1], BgCol[2], 255);
        SDL_RenderClear(_renderer);

        // GRID
        {
            SDL_SetRenderDrawColor(_renderer, BgCol[0] + 150, BgCol[1] + 120,
                                   BgCol[2] * 0, 255);

            int x1, y1, x2, y2;

            // vertical lines
            for (int i = 0; i < Cols + 1; ++i) {
                // printf("");
                x1 = i * TILE_WIDTH;
                y1 = 0;
                x2 = x1;
                y2 = y1 + SCR_HEIGHT;
                SDL_RenderDrawLine(_renderer, x1, y1, x2, y2);
            }
            // horizontal lines
            for (int i = 0; i < Rows + 1; ++i) {
                x1 = 0;
                y1 = i * TILE_HEIGHT;
                x2 = x1 + SCR_WIDTH;
                y2 = y1;
                SDL_RenderDrawLine(_renderer, x1, y1, x2, y2);
            }
        }

        // LEVEL
        {
            // TILES
            SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);

            const int pad = 12;

            for (int i = 0; i < TileCount; ++i) {
                tile* t = &Tiles[i];
                if (!t->IsAnimating && t->Hit) continue;  // skip non-animating hit tiles
                const int x      = pad + t->X;
                const int y      = pad + t->Y;
                const int w      = t->W;
                const int h      = t->H;
                SDL_Rect TexRect = texture_rect(t->TexRow, t->TexCol);

                SDL_Rect TileRect = {x, y, w - 2 * pad, h - 2 * pad};
                SDL_RenderCopy(_renderer, Texture, &TexRect, &TileRect);
            }
        }


        // PLAYER
        {
            SDL_Rect PlayerRect = RECT(Player);
            SDL_Rect TexRect    = texture_rect(1, 1);  // TODO
            SDL_RenderCopy(_renderer, Texture, &TexRect, &PlayerRect);
        }
        // BALL
        {

            SDL_Rect BallRect = RECT(Ball);
            SDL_SetRenderDrawColor(_renderer, 25, 25, 255, 255);
            SDL_RenderFillRect(_renderer, &BallRect);
        }

        // "EFFECTS"
        {
            effect_hl_nearest_tile();
        }

        // UI
        {
            // score
            {

                SDL_Texture* score_texture;
                SDL_Rect score_texture_rect;
                ui_score(Score, &score_texture, &score_texture_rect);
                SDL_RenderCopy(_renderer, score_texture, NULL, &score_texture_rect);
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
                ui_fps(fps, &fps_texture, &fps_texture_rect);
                SDL_RenderCopy(_renderer, fps_texture, NULL, &fps_texture_rect);
            }
        }


        SDL_RenderPresent(_renderer);
        // END DRAWING

        // check win/lose condition
        if (is_game_over()) {
            puts("Game Over");
            puts("You Win!");
            printf("Score: %d\n", Score);
            const int in_secs = Time / 1000;
            const int mins    = in_secs / 60;
            const int secs    = in_secs % 60;
            printf("Time: %d mins and %d secs!\n", mins, secs);
            GameRunning = 0;
            continue;
        }

        SDL_Delay(1000 / 60);  // fps
    }


    return 0;
}


// return 1 if game over/player won
int is_game_over()
{
    if (Score < TileCount)
        return 0;  // not won
    else
        return 1;
}

void on_tile_got_hit(tile* t)
{
    t->Hit++;
    t->IsAnimating = 1;  // start animating
    Score++;             // inc the score
}

void update_state(const Uint8* Keys)
{
    // slow motion!
    effect_slowdown_player(Keys);
    const int playerspeed = Player->Vel.X;


    // player movement
    if (Keys[SDL_SCANCODE_A]) {
        Player->X -= playerspeed;  // move left
    }
    if (Keys[SDL_SCANCODE_D]) {
        Player->X += playerspeed;  // move right
    }

    // player actions
    if (Keys[SDL_SCANCODE_SPACE]) { puts("launch ball!"); }

    // player-level collision detection
    if (LEFT(Player) < 0) { Player->X = 0; }
    if (RIGHT(Player) > SCR_WIDTH) { Player->X = SCR_WIDTH - Player->W; }


    // ball update
    Ball->X += Ball->Vel.X;
    Ball->Y += Ball->Vel.Y;


    // ball-level collision
    if (LEFT(Ball) < 0) {
        Ball->X     = 0;
        Ball->Vel.X = -Ball->Vel.X;
    } else if (RIGHT(Ball) > SCR_WIDTH) {
        Ball->X     = SCR_WIDTH - Ball->W;
        Ball->Vel.X = -Ball->Vel.X;
    }
    if (TOP(Ball) < 0) {
        Ball->Y     = 0;
        Ball->Vel.Y = -Ball->Vel.Y;
    } else if (BOTTOM(Ball) > SCR_HEIGHT) {
        Ball->Y     = SCR_HEIGHT - Ball->H;
        Ball->Vel.Y = -Ball->Vel.Y;
    }

    // Ball-Player check
    {
        SDL_Rect ball_rect   = RECT(Ball);
        SDL_Rect player_rect = RECT(Player);

        SDL_Rect tile_rect;
        for (int i = 0; i < TileCount; ++i) {
            tile* t = &Tiles[i];
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
                Ball->Vel.X = new_xspeed;    // based on offset
                Ball->Vel.Y = -Ball->Vel.Y;  // just changes direction in Y

                // perform the procedure
                on_tile_got_hit(t);
                break;
            }
        }

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
            Ball->Vel.X = new_xspeed;    // based on offset
            Ball->Vel.Y = -Ball->Vel.Y;  // just changes direction in Y
        }
    }
}

// create SDL_Window and SDL_Renderer
int _init_sdl(int Width, int Height, SDL_Window** outWin, SDL_Renderer** outRenderer)
{
    // SDL proper
    int err = 0;
    err     = SDL_Init(SDL_INIT_EVERYTHING);
    if (err) {
        printf("SDL2 Error\n");
        exit(1);
    }
    err = SDL_CreateWindowAndRenderer(Width, Height, 0, outWin, outRenderer);
    if (err) {
        printf("SDL2 Window Error\n");
        _deinit_sdl();
    }

    // sdl image
    const int flags   = IMG_INIT_JPG | IMG_INIT_PNG;
    const int initted = IMG_Init(flags);
    if ((initted & flags) != flags) {
        puts("Failed to init SDL_Image");
        _deinit_sdl();
        exit(1);
    }

    // sdl ttf

    err = TTF_Init();
    if (err) {
        puts("Failed to init SDL_ttf");
        _deinit_sdl();
        exit(1);
    }


    return 0;
}

int _deinit_sdl()
{
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
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

SDL_Texture* load_texture(const char* path)
{
    SDL_Surface* surf = IMG_Load(path);
    if (surf == NULL) {
        printf("Failed to load image: %s\n", path);
        _deinit_sdl();
        exit(1);
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surf);
    if (texture == NULL) {
        printf("Failed to convert SDL_Surface to SDL_Texture: %s\n", path);
        printf("ERROR: %s\n", SDL_GetError());
        SDL_FreeSurface(surf);
        _deinit_sdl();
        exit(1);
    }
    SDL_FreeSurface(surf);
    return texture;
}


#define SAVEFILENAME "gamestate.bin"

// return SDL_Rect of sprite at (col, row)
SDL_Rect texture_rect(unsigned int col, unsigned int row)
{
    const int texwidth   = 80;
    const int texheight  = 40;
    const int rowcount   = 5;
    const int colcount   = 5;
    const int rectwidth  = texwidth / colcount;
    const int rectheight = texheight / rowcount;

    const int valid_index = (col < colcount) && (row < rowcount);
    if (!valid_index) { col = row = 0; }  // default value

    const int x = (col * rectwidth);
    const int y = (row * rectheight);

    SDL_Rect Result = {x, y, rectwidth, rectheight};
    return Result;
}


/** level
 *      ************
 *      * - - - - -*
 *      * - - - - -*    <- Tiles
 *      * - - - - -*
 *      * - -   - -*
 *      * -     - -*
 *      *          *
 *      *     o    *    <- ball
 *      *   ----   *    <- Playerline and player
 *      ************
 *
 * Steps:
 *  - init
 *  loop:
 *      - take user input and move player
 *      - move ball and tiles
 *      - check for collisions
 *      - update tiles and score
 *      - apply effects
 *      - draw BG, TILES, BALL, PLAYER, UI(SCORE)
 */

/** NOTES:
 * naming:
 *  - global vars are capitalized: Tiles, Player etc.
 *  - locals and params are snake_case
 *  - functions are snake_case
 */

/** TODO:
- add object collision
- score
- fix level dimension and other minor details
- load and save gamestate
- slow motion
- effects
*/

/** BUGS:
- you tell me!
*/
