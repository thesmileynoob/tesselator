#pragma once
#include <SDL2/SDL.h>

/**
TODO:
- add level-grid
- add ball-player collision
- add ball-tile collision
- fix level dimension and other minor details
- add ball and its physics
- collision detection
- load and save gamestate
- slow motion

BUGS:
- you tell me!
*/

/** level
 *      ************
 *      * - - - - -*
 *      * - - - - -*    <- Tiles
 *      * - - - - -*
 *      * - -   - -*
 *      * -     - -*
 *      *          *
 *      *     o    *    <- ball
 *      *   ----   *    <- Playerline
 *      ************
 */

#define SCREENWIDTH 780
#define SCREENHEIGHT 800
#define TILEWIDTH 10 * 16
#define TILEHEIGHT 10 * 8

typedef struct level {
        int* Tiles;
        int Rows;
        int Cols;

        /// multiple of 16 cuz of chosen texture
        int TileWidth;
        int TileHeight;
        int Pad;

        int PlayerLine;    // y offset from the top

        SDL_Texture* Texture;    // texture sheet
} level;


typedef struct player {
        int X, Y;
        int Width, Height;
} player;

typedef struct ball {
        int X, Y;
        int Width, Height;
} ball;


typedef struct gamestate {
        // env
        int Running;    // game running flag

        player Player;
        ball Ball;
        level Level;
} gamestate;


// Helpful macros
#define TOP(Obj) Obj->Y
#define BOTTOM(Obj) Obj->Y + Obj->Height
#define LEFT(Obj) Obj->X
#define RIGHT(Obj) Obj->X + Obj->Width
#define RECT(Obj) \
        (SDL_Rect) { Obj->X, Obj->Y, Obj->Width, Obj->Height }


SDL_Texture* load_texture(const char* path);
unsigned int get_dt();
level load_level(int number);
