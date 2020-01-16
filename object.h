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

typedef struct object {
        int Xpos, Ypos;
        int Width, Height;

        int Xspeed, Yspeed;
        int JumpSpeed;

        enum obj_state State;

        char r, g, b;
} object;


typedef struct tile {
        int Xpos, Ypos;
        int Width, Height;

        char r, g, b;
} tile;



void reset_player(object* Player, int xpos, int ypos);

int tile_below_object(const object*, const tile*, int TileCount);
