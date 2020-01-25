#pragma once
#include <SDL2/SDL.h>

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

