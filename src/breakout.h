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

#define BACKGROUND_TILE_COUNT 16  // "assets/bg/{1.png, 2.png,... 16.png}"

// game
unsigned int get_dt();
void update_state(const Uint8* Keys);
