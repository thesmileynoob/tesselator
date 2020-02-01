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

    // return true if object is inside these bounds
    virtual bool IsInside(float left, float right, float top, float bottom);
};

namespace game
{

extern int level_left;
extern int level_right;
extern int level_top;
extern int level_bottom;

}  // namespace game
