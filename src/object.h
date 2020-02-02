#pragma once
#include "breakout.h"
#include "gfx.h"
#include "math.h"


// ball, tile and player are objects
struct object {
public:
    // general
    int X, Y, W, H;
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

    // inside game::* bounds
    bool IsInside();
    // inside given bounds
    virtual bool IsInside(float left, float right, float top, float bottom);
};
