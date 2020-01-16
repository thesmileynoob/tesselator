#pragma once
/*
TODO
- add obstacles in the level
- collision detection
- slow motion
*/

enum obj_state {
        STANDING,
        JUMPING,
        RUNNING,
};

typedef struct object {
        int Xpos, Ypos;
        int Width, Height;

        int Xspeed, Yspeed;
        int JumpSpeed;

        enum obj_state State;

        char r, g, b;
} object;
