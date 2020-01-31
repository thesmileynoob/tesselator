#pragma once

typedef struct vec2 {
    float X, Y;
    float _X, _Y;  // original values
} vec2;

struct object;
typedef struct animation {
    int (*anim_func)(struct object*);
    int frame_count;
} animation;

// ball, tile and player are objects
typedef struct object {
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
} object, tile;
