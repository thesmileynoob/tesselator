#include "object.h"

// use this to change velocities
void apply_force(object* Obj, int fx, int fy)
{
        Obj->Xspeed += fx;
        Obj->Yspeed += fy;
}

// use this to set positions
void set_pos(object* Obj, int x, int y)
{
        Obj->Xpos = x;
        Obj->Ypos = y;
}

void set_state(object* Obj, enum obj_state State)
{
        enum obj_state oldState = Obj->State;
        Obj->State              = State;
        if (State != oldState) {
                printf("Setting state: %s -> %s\n", obj_state_str[oldState],
                       obj_state_str[State]);
        }
}

void jump(object* Obj, gamestate* gs)
{
        // values to be assigned
        int new_Xpos, new_Ypos;

        if (Obj->State != JUMPING) { set_state(Obj, JUMPING); }

        // apply gravity
        apply_force(Obj, 0, gs->Gravity);
        new_Xpos = Obj->Xpos + Obj->Xspeed;
        new_Ypos = Obj->Ypos + Obj->Yspeed;

        int Ydisp = abs(Obj->Yspeed);


        if (Obj->Yspeed < 0) {
                // rising up
                new_Ypos -= Ydisp;
        } else {
                // falling down
                new_Ypos += Ydisp;
        }

        // update positons
        set_pos(Obj, new_Xpos, new_Ypos);
}

void predict_position(const object* Obj, int* newX, int* newY)
{

        *newX = Obj->Xpos + Obj->Xspeed;
        *newY = Obj->Ypos + Obj->Yspeed;
}

void idle(object* Obj, const tile* TileBelow)
{
        if (Obj->State != IDLE) { set_state(Obj, IDLE); }
        return;    // TODO: TEMP

        // move with the tile
        int newTileX, newTileY;
        predict_position(TileBelow, &newTileX, &newTileY);
        const int dx = newTileX - TileBelow->Xpos;
        const int dy = newTileY - TileBelow->Ypos;
        printf("d %d, %d\n", dx, dy);

        const int newXPos = Obj->Xpos + dx;
        const int newYPos = Obj->Ypos + dy;
        set_pos(Obj, newXPos, newYPos);
}
