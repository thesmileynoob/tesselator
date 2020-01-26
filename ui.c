#include "breakout.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/// TODO: Right now these are defined in main.c
extern SDL_Renderer* _renderer;
extern TTF_Font* UIFont;

/** create a texture and its rect of the score string */
void ui_score(int score, SDL_Texture** outScoreTexture, SDL_Rect* outScoreRect)
{
    static SDL_Texture* score_texture = NULL;
    static SDL_Rect score_rect;
    static int last_score = -1;  // = -1 so that score_changed is true the first time

    int score_changed = last_score != score;

    // re-construct the score_texture iff score has changed
    if (score_changed) {
        // printf("UI re-constructing score texture!\n");
        last_score = score;  // update last score

        // cleanup previous texture
        if (score_texture) SDL_DestroyTexture(score_texture);

        // construct the string to be displayed
        const int max_len = 15;
        char score_string[max_len];
        snprintf(score_string, max_len, "Score: %d", score);

        // make a texture of the string
        SDL_Surface* surface =
            TTF_RenderText_Solid(UIFont, score_string, (SDL_Color){255, 255, 255});
        if (!surface) { printf("UIError: %s\n", TTF_GetError()); }

        score_texture = SDL_CreateTextureFromSurface(_renderer, surface);
        if (!score_texture) { printf("UIError: %s\n", SDL_GetError()); }

        SDL_FreeSurface(surface);  // not needed now

        // update score_rect
        score_rect.x = score_rect.y = 0;
        TTF_SizeText(UIFont, score_string, &score_rect.w, &score_rect.h);
    }
    assert(score_texture);

    *outScoreTexture = score_texture;
    *outScoreRect    = score_rect;
}
