#pragma once
#include <cassert>

#include <SDL2/SDL_rect.h>

struct vec2 {
public:
    float X, Y;
    float _X, _Y;  // original values

public:
    vec2();
    vec2(float x, float y);

    void scale(float scale);
    void reset();
};


// scale a rect without changing its center
inline SDL_Rect scale_sdl_rect(SDL_Rect rect, const float scale)
{
    assert(scale > 1.);

    // scaled w,h
    const int sw = scale * rect.w;
    const int sh = scale * rect.h;

    // difference b/w original and scaled w,h
    const int dw = sw - rect.w;
    const int dh = sh - rect.h;

    rect.x -= dw / 2;
    rect.y -= dh / 2;
    rect.w = sw;
    rect.h = sh;

    return rect;
}
