#include <SDL2/SDL_keycode.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "breakout.h"


// Constants
#define SCR_WIDTH 780
#define SCR_HEIGHT 900
#define TILE_WIDTH 10 * 16
#define TILE_HEIGHT 10 * 8

// Helpful macros
#define TOP(Obj) Obj->Y
#define BOTTOM(Obj) Obj->Y + Obj->H
#define LEFT(Obj) Obj->X
#define RIGHT(Obj) Obj->X + Obj->W
#define RECT(Obj) \
    (SDL_Rect) { Obj->X, Obj->Y, Obj->W, Obj->H }


// ball, tile and player are objects
typedef struct object {
    int X, Y, W, H;
    const char* Name;

    // texture
    int TexRow, TexCol;
} object, tile;


// globals
int GameRunning;  // game running flag

int Cols      = 5;
int Rows      = 6;
int TileCount = 5 * 6;
tile* Tiles   = NULL;

object* Player;

object* Ball;
int Xspeed = 2;
int Yspeed = 8;

SDL_Texture* Texture;
Uint8 BgCol[3];  // r,g,b


// env
SDL_Window* _window     = NULL;
SDL_Renderer* _renderer = NULL;


/** here we go! */
int main(int argc, char const* argv[])
{
    // INIT
    {
        _init_sdl(SCR_WIDTH, SCR_HEIGHT, &_window, &_renderer);
        assert(_window && _renderer);

        GameRunning = 1;
        Texture     = load_texture("../assets/tiles.png");
        assert(Texture);

        // alloc
        Player = calloc(1, sizeof(object));
        Ball   = calloc(1, sizeof(object));
        Tiles  = calloc(TileCount, sizeof(object));
        assert(Player && Ball && Tiles);

        int xoff, yoff;
        xoff = yoff = 0;
        for (int i = 0; i < TileCount; ++i) {
            tile* t   = &Tiles[i];
            t->X      = xoff;
            t->Y      = yoff;
            t->W      = TILE_WIDTH;
            t->H      = TILE_HEIGHT;
            t->TexRow = i % 5;
            t->TexCol = (i + 2) % 5;

            xoff += TILE_WIDTH;
            if (xoff > SCR_WIDTH) {
                xoff = 0;
                yoff += TILE_HEIGHT;
            }
        }


        // init gamestate
        // player
        Player->X = SCR_WIDTH / 2;
        Player->Y = 4.2 / 5.0 * SCR_HEIGHT;
        Player->W = 155;
        Player->H = 35;

        // ball
        Ball->X = Player->X + 40;  // TODO: 40
        Ball->Y = Player->Y - 50;
        Ball->W = 25;
        Ball->H = 25;
    }


    while (GameRunning) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            SDL_Keycode sym = ev.key.keysym.sym;
            if (sym == SDLK_q || ev.type == SDL_QUIT) { GameRunning = 0; }

            if (ev.type == SDL_KEYUP) {
                if (sym == SDLK_RETURN) { Rows++; }
            } else if (ev.type == SDL_KEYDOWN) {
            }
        }

        // handle input and update state
        update_state(SDL_GetKeyboardState(NULL));

        // START DRAWING
        SDL_SetRenderDrawColor(_renderer, BgCol[0], BgCol[1], BgCol[2], 255);
        SDL_RenderClear(_renderer);

        // GRID
        {
            int timeout;
            if (timeout++ > 100) {
                BgCol[0] = SDL_GetTicks() % 255;
                BgCol[1] = 10;
                BgCol[2] = 100;
            }
            if (timeout > 100) { timeout = 0; }
            SDL_SetRenderDrawColor(_renderer, BgCol[0], BgCol[1], BgCol[2], 255);

            int x1, y1, x2, y2;

            // vertical lines
            for (int i = 0; i < Cols; ++i) {
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
                tile* t          = &Tiles[i];
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


        SDL_RenderPresent(_renderer);
        // END DRAWING

        SDL_Delay(1000 / 60);  // fps
    }


    return 0;
}


void update_state(const Uint8* Keys)
{
    // update player
    const int playerspeed = 15;

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
    Ball->X += Xspeed;
    Ball->Y += Yspeed;


    // ball-level collision
    if (LEFT(Ball) < 0) {
        Ball->X = 0;
        Xspeed  = -Xspeed;
    } else if (RIGHT(Ball) > SCR_WIDTH) {
        Ball->X = SCR_WIDTH - Ball->W;
        Xspeed  = -Xspeed;
    }
    if (TOP(Ball) < 0) {
        Ball->Y = 0;
        Yspeed  = -Yspeed;
    } else if (BOTTOM(Ball) > SCR_HEIGHT) {
        Ball->Y = SCR_HEIGHT - Ball->H;
        Yspeed  = -Yspeed;
    }

    // Ball-Player check
    {
        SDL_Rect ball_rect   = RECT(Ball);
        SDL_Rect player_rect = RECT(Player);

        SDL_Rect tile_rect;
        for (int i = 0; i < TileCount; ++i) {
            tile* t   = &Tiles[i];
            tile_rect = RECT(t);
            if (SDL_HasIntersection(&ball_rect, &tile_rect) == SDL_TRUE) {
                const int ball_center = ball_rect.x + (ball_rect.w / 2);
                const int tile_center = tile_rect.x + (tile_rect.w / 2);
                const int max_xspeed  = 7;

                // -ve means ball to the left
                const int offset = (ball_center - tile_center) / 2;

                float scale    = 0.2;  // TODO: Better name
                int new_xspeed = scale * offset;
                if (new_xspeed > max_xspeed) new_xspeed = max_xspeed;    // clip max
                if (new_xspeed < -max_xspeed) new_xspeed = -max_xspeed;  // clip max

                // printf("%d -> %d\n", Xspeed, new_xspeed);
                Xspeed = new_xspeed;  // based on offset
                Yspeed = -Yspeed;     // just changes direction in Y
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

            // printf("%d -> %d\n", Xspeed, new_xspeed);
            Xspeed = new_xspeed;  // based on offset
            Yspeed = -Yspeed;     // just changes direction in Y
        }
    }
}

// create SDL_Window and SDL_Renderer
int _init_sdl(int Width, int Height, SDL_Window** outWin, SDL_Renderer** outRenderer)
{
    // init SDL proper
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

    // init sdl image
    const int flags   = IMG_INIT_JPG | IMG_INIT_PNG;
    const int initted = IMG_Init(flags);
    if ((initted & flags) != flags) {
        puts("Failed to init SDL_Image");
        _deinit_sdl();
    }

    return 0;
}

int _deinit_sdl()
{
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
 *      - draw BG, TILES, BALL, PLAYER, SCORE
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
