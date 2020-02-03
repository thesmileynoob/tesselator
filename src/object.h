#pragma once
#include "breakout.h"
#include "gfx.h"
#include "math.h"
#include "animation.h"


// ball, tile and player are objects
struct object {
public:
    // absolute coordinates
    int X, Y;
    int W, H;
    const char* Name;

    // gameplay
    int Hit;
    vec2 Vel;

    // graphics & texture
    int TexRow, TexCol;

    int Hidden;

public:
    object();

    virtual void Update();
    virtual void Draw();

    // relative to game::
    int Left() const;
    int Right() const;
    int Top() const;
    int Bottom() const;
    SDL_Rect Rect() const;
    vec2 Center() const;

    // relative to window
    int AbsLeft() const;
    int AbsRight() const;
    int AbsTop() const;
    int AbsBottom() const;
    SDL_Rect AbsRect() const;
    vec2 AbsCenter() const;

    // inside game::* bounds
    bool IsInside();
    // inside given bounds
    virtual bool IsInside(float left, float right, float top, float bottom);
};
