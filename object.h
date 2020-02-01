#pragma once

#include "animation.h"
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

    // animation
    int IsAnimating;
    animation Anim;

    // graphics & texture
    int TexRow, TexCol;

    int Hidden;

public:
    object();

    virtual void Update();
    virtual void Draw();
    virtual int Animate();

    // return true if object is inside these bounds
    virtual bool IsInside(float left, float right, float top, float bottom);
};
