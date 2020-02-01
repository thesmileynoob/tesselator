#pragma once
#include <SDL2/SDL.h>

namespace ui
{
void gen_fps(float fps, SDL_Texture** outTexture, SDL_Rect* outRect);
void gen_score(int score, SDL_Texture** outTexture, SDL_Rect* outRect);
}  // namespace ui
