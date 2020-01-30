#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct vec2 {
    float X, Y;
    float _X, _Y;  // original values
} vec2;

struct object;

typedef struct animation {
    int (*anim_func)(struct object*);
    int frame_count;
} animation;

// ball, tile and player are objects
typedef struct object {
    // general
    int X, Y, W, H;
    const char* Name;

    // gameplay
    int Hit;
    vec2 Vel;

    // animation
    int IsAnimating;
    animation Anim;

    // graphics & texture
    int TexRow, TexCol;


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

#define CENTER_X(Obj) LEFT(Obj) + (Obj->W / 2);
#define CENTER_Y(Obj) TOP(Obj) + (Obj->H / 2);

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

// ui
void ui_score(int score, SDL_Texture** outScoreTexture, SDL_Rect* outScoreRect);
void ui_fps(float fps, SDL_Texture** outTexture, SDL_Rect* outRect);

// math
vec2 vec2_create(float x, float y);
void vec2_scale(vec2* v, float scale);
void vec2_reset(vec2* v);
