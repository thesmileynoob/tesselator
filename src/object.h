#pragma once
#include "gfx.h"
#include "math.h"


// ball, tile and player are objects
struct object {
public:
    // absolute coordinates
    float X, Y;
    float W, H;
    const char* Name;

    // gameplay
    int Hit;
    vec2 Vel;

    // graphics & texture
    int TexRow, TexCol;

    int Hidden;

public:
    object();

    virtual void update();
    virtual void draw();

    // relative to game::
    float left() const;
    float right() const;
    float top() const;
    float bottom() const;
    SDL_Rect rect() const;
    vec2 center() const;

    // relative to window
    float abs_left() const;
    float abs_right() const;
    float abs_top() const;
    float abs_bottom() const;
    SDL_Rect abs_rect() const;
    vec2 abs_center() const;
    //

    // inside game::* bounds
    bool is_within_world();
    // inside given bounds
    virtual bool is_inside(float left, float right, float top, float bottom);
};
