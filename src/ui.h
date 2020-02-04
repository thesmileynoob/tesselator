#pragma once
#include <SDL2/SDL.h>

namespace ui
{
// fps text
void gen_fps(float fps, SDL_Texture** outTexture, SDL_Rect* outRect);

// score text
void gen_score(int score, SDL_Texture** outTexture, SDL_Rect* outRect);

// time text
void gen_time(unsigned int time_ms, SDL_Texture** outTexture, SDL_Rect* outRect);

// player lost a life text
void gen_player_lose_life(SDL_Texture** outTexture, SDL_Rect* outRect);
}  // namespace ui
