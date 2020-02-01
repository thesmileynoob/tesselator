#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "gfx.h"
#include "breakout.h"


namespace ui
{

/** create a texture and its rect of the score string */
void gen_score(int score, SDL_Texture** outTexture, SDL_Rect* outRect)
{
    static SDL_Texture* texture = NULL;
    static SDL_Rect rect;
    static int last_score = -1;  // = -1 so that score_changed is true the first time

    int score_changed = last_score != score;

    // re-construct the score_texture iff score has changed
    if (score_changed) {
        // printf("UI re-constructing score texture!\n");
        last_score = score;  // update last score

        // cleanup previous texture
        if (texture) SDL_DestroyTexture(texture);

        // construct the string to be displayed
        const int max_len = 15;
        char string[max_len];
        snprintf(string, max_len, "Score: %d", score);

        // make a texture of the string
        SDL_Surface* surface =
            TTF_RenderText_Solid(gfx::UIFont, string, (SDL_Color){255, 255, 255});
        if (!surface) { printf("UIError: %s\n", TTF_GetError()); }

        texture = SDL_CreateTextureFromSurface(gfx::_renderer, surface);
        if (!texture) { printf("UIError: %s\n", SDL_GetError()); }

        SDL_FreeSurface(surface);  // not needed now

        // update score_rect
        rect.x = rect.y = 0;
        TTF_SizeText(gfx::UIFont, string, &rect.w, &rect.h);
    }
    assert(texture);

    *outTexture = texture;
    *outRect    = rect;
}

void gen_fps(float fps, SDL_Texture** outTexture, SDL_Rect* outRect)
{
    static SDL_Texture* texture = NULL;
    static SDL_Rect rect;
    const int timeout_len = 50;
    static int timeout    = 0;  // recalculate once in 'timeout_len' iterations

    // re-construct the score_texture iff timeout = 0
    if (timeout-- <= 0) {
        timeout = timeout_len;  // reset timeout
        // printf("UI re-constructing fps texture!\n");

        // cleanup previous texture
        if (texture) SDL_DestroyTexture(texture);

        // construct the string to be displayed
        const int max_len = 15;
        char string[max_len];
        snprintf(string, max_len, "%.1f FPS", fps);

        // make a texture of the string
        SDL_Surface* surface =
            TTF_RenderText_Solid(gfx::UIFont, string, (SDL_Color){255, 255, 255});
        if (!surface) { printf("UIError: %s\n", TTF_GetError()); }

        texture = SDL_CreateTextureFromSurface(gfx::_renderer, surface);
        if (!texture) { printf("UIError: %s\n", SDL_GetError()); }

        SDL_FreeSurface(surface);  // not needed now

        // update score_rect
        TTF_SizeText(gfx::UIFont, string, &rect.w, &rect.h);
        rect.x = SCR_WIDTH - rect.w - 10;  // 10 padding
        rect.y = 0;
    }
    assert(texture);

    *outTexture = texture;
    *outRect    = rect;
}

}  // namespace ui
