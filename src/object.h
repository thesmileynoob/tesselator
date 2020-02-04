#pragma once
#include "breakout.h"
#include "gfx.h"
#include "math.h"
#include "animation.h"


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

    virtual void Update();
    virtual void Draw();

    // relative to game::
    float Left() const;
    float Right() const;
    float Top() const;
    float Bottom() const;
    SDL_Rect Rect() const;
    vec2 Center() const;

    // relative to window
    float AbsLeft() const;
    float AbsRight() const;
    float AbsTop() const;
    float AbsBottom() const;
    SDL_Rect AbsRect() const;
    vec2 AbsCenter() const;

    // inside game::* bounds
    bool IsWithinWorld();
    // inside given bounds
    virtual bool IsInside(float left, float right, float top, float bottom);
};
