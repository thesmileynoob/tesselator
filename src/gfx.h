#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_ttf.h>

namespace gfx
{
// env
extern SDL_Window* _window;
extern SDL_Renderer* _renderer;
extern SDL_Texture* Texture;
extern TTF_Font* UIFont;


void init(int width, int height);
void deinit();

void draw_texture(SDL_Texture* t, SDL_Rect* src, SDL_Rect* dst);

SDL_Texture* load_texture(const char* path);

SDL_Rect texture_rect(unsigned int col, unsigned int row);
}  // namespace gfx