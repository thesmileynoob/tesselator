#pragma once
#include <SDL2/SDL.h>

/**
TODO:
- add obstacles in the level
- collision detection
- slow motion
- load and save gamestate

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

static const char* obj_state_str[] = {
    "IDLE",
    "JUMPING",
    "RUNNING",
    "FALLING",
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


typedef struct gamestate {
        // env
        int ScreenWidth;
        int ScreenHeight;
        int GroundLevel;
        int Gravity;
        int Running;

        // objects
        object* Player;
        tile* Tiles;
        int TileCount;

        // dev/debug
        int visual_debug;
        tile* highlighted_tile;
} gamestate;


// Helpful macros
#define TOP(Obj) Obj->Ypos
#define BOTTOM(Obj) Obj->Ypos + Obj->Height
#define LEFT(Obj) Obj->Xpos
#define RIGHT(Obj) Obj->Xpos + Obj->Width
#define RECT(Obj) \
        (SDL_Rect) { Obj->Xpos, Obj->Ypos, Obj->Width, Obj->Height }


void player_reset(object* Player, int xpos, int ypos);

unsigned int get_dt();
void step_player(gamestate* gs, unsigned int dt);
void step_tiles(gamestate* gs, unsigned int dt);

// return NULL if a tile isn't present below
tile* tile_below_object(const object* Obj, const tile* Tiles, int TileCount);
static SDL_Texture* load_texture(const char* path);

// physics.c
void apply_force(object* Obj, int fx, int fy);
void set_pos(object* Obj, int x, int y);
void set_state(object* Obj, enum obj_state State);
void jump(object* Obj, gamestate* gs);
void idle(object* Obj, const tile* TileBelow);
