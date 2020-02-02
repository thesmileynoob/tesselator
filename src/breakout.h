#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


// Constants
#define SCR_WIDTH 780  // NOTE: touching this causes bugs
#define SCR_HEIGHT (int) (1.3 * SCR_WIDTH)
#define FRAME_WIDTH 30
#define _real_tile_width 16  // px
#define _real_tile_height 8  // px
#define TILE_WIDTH (9 * _real_tile_width)
#define TILE_HEIGHT (9 * _real_tile_height)

// Helpful macros
#define TOP(Obj) Obj->Y
#define BOTTOM(Obj) Obj->Y + Obj->H
#define LEFT(Obj) Obj->X
#define RIGHT(Obj) Obj->X + Obj->W

#define CENTER_X(Obj) LEFT(Obj) + (Obj->W / 2);
#define CENTER_Y(Obj) TOP(Obj) + (Obj->H / 2);

#define RECT(Obj) \
    (SDL_Rect) { Obj->X, Obj->Y, Obj->W, Obj->H }

// game
unsigned int get_dt();
void update_state(const Uint8* Keys);
