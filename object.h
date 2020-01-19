#pragma once
/*
TODO
- add obstacles in the level
- collision detection
- slow motion
*/

enum obj_state {
        IDLE,
        JUMPING,
        RUNNING,
        FALLING,
};

enum obj_type {
        PLAYER,
        TILE,
};

typedef struct object {
        int Xpos, Ypos;
        int Width, Height;

        int Xspeed, Yspeed;
        int JumpSpeed;

        enum obj_state State;
        enum obj_type Type;

        char r, g, b;

} object, tile;


#define TOP(Obj) Obj->Ypos
#define BOTTOM(Obj) Obj->Ypos + Obj->Height
#define LEFT(Obj) Obj->Xpos
#define RIGHT(Obj) Obj->Xpos + Obj->Width
#define RECT(Obj) (SDL_Rect){Obj->Xpos, Obj->Ypos, Obj->Width, Obj->Height}

void reset_player(object* Player, int xpos, int ypos);

int tile_below_object(const object*, const tile*, int TileCount);
