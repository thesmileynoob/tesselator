#pragma once
#include <SDL2/SDL.h>

/**
TODO:
- add level-grid
- add ball-player collision
- add ball-tile collision
- fix level dimension and other minor details
- add ball and its physics
- collision detection
- load and save gamestate
- slow motion

BUGS:
- you tell me!
*/

enum obj_state {
        IDLE,
        MOVING,
        COLLIDING,
};

extern const char* obj_state_str[];

enum obj_type {
        PLAYER,
        TILE,
        BALL,
};


typedef struct object {
        int Xpos, Ypos;
        int Width, Height;

        enum obj_type Type;
        enum obj_state State;

        SDL_Texture* Texture;
} object, tile;


typedef struct gamestate {
        // env
        int ScreenWidth;
        int ScreenHeight;
        int Running;    // game running flag

        // level spec
        int CurrentLevel;
        int GroundLevel;    /// separate player from the rest of tile area
        int TileWidth;     /// game world tile width. multiple of 16 cuz of chosen texture
        int TileHeight;    /// game world tile height.
        int TileXgap;
        int TileYgap;

        // level objects
        object* Player;
        object* Ball;
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


SDL_Texture* load_texture(const char* path);

void player_reset(object* Player, int xpos, int ypos);

unsigned int get_dt();
void step_player(gamestate* gs, unsigned int dt);
void step_tiles(gamestate* gs, unsigned int dt);

// return NULL if a tile isn't present below
tile* tile_below_object(const object* Obj, tile* Tiles, int TileCount);
