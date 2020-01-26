#pragma once
#include <SDL2/SDL.h>

struct object;

typedef struct animation {
    int (*anim_func)(struct object*);
    int frame_count;
} animation;

// ball, tile and player are objects
typedef struct object {
    int X, Y, W, H;
    const char* Name;

    // texture
    int TexRow, TexCol;

    int Hit;

    int IsAnimating;
    animation Anim;


} object, tile;


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

#define CENTER_X(Obj) LEFT(Obj) + (Obj->W/2);
#define CENTER_Y(Obj) TOP(Obj) + (Obj->H/2);

#define RECT(Obj) \
    (SDL_Rect) { Obj->X, Obj->Y, Obj->W, Obj->H }


// sdl
int _init_sdl(int Width, int Height, SDL_Window** outWin, SDL_Renderer** outRenderer);
int _deinit_sdl();

// assets
SDL_Texture* load_texture(const char* path);
SDL_Rect texture_rect(unsigned int col, unsigned int row);

// game
unsigned int get_dt();
void update_state(const Uint8* Keys);
int is_game_over();
void reset_level();

// animations
int anim_tile_breakout_animation(tile* t);
int anim_player_expand(tile* t);
