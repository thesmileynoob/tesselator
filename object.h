#pragma once

#include "animation.h"

typedef struct vec2 {
    float X, Y;
    float _X, _Y;  // original values
} vec2;


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
    virtual void Update();
    virtual void Draw();
};
typedef object tile;
