#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "object.h"
#include "tile.h"


// Constants
#define SCR_WIDTH 780  // NOTE: touching this causes bugs
#define SCR_HEIGHT 1.3 * SCR_WIDTH
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
