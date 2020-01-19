#pragma once
#include <SDL2/SDL.h>

/**
TODO:
- add obstacles in the level
- collision detection
- slow motion

BUGS:
- player falls through when standing on a tile
  that is moving up.
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

        int FaceRight;

        enum obj_state State;
        enum obj_type Type;

        char r, g, b;
        SDL_Texture* Texture;

} object, tile;

// Globals
#define ScreenWidth 1280
#define ScreenHeight 720
#define GroundLevel (int) ScreenHeight * 2 / 4
#define gravity 2

// Helpful macros
#define TOP(Obj) Obj->Ypos
#define BOTTOM(Obj) Obj->Ypos + Obj->Height
#define LEFT(Obj) Obj->Xpos
#define RIGHT(Obj) Obj->Xpos + Obj->Width
#define RECT(Obj) \
        (SDL_Rect) { Obj->Xpos, Obj->Ypos, Obj->Width, Obj->Height }


void player_reset(object* Player, int xpos, int ypos);

// return NULL if a tile isn't present below
tile* tile_below_object(const object* Obj, const tile* Tiles, int TileCount);
