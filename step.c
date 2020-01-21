#include "object.h"

void step_player(gamestate* gs, unsigned int dt)
{
        // object* Obj = gs->Player;

        // // check if there's a tile below object
        // gs->highlighted_tile =
        //     tile_below_object(Obj, gs->Tiles, gs->TileCount);    // NULLABLE

        // // Collect data required for the step_player
        // const tile* TileBelow = gs->highlighted_tile;    // NULLABLE

        // // new values to be assigned to the boject
        // int new_Xpos, new_Ypos;
        // new_Xpos = Obj->Xpos + Obj->Xspeed;

        // // apply gravity
        // apply_force(Obj, 0, gs->Gravity);
        // new_Ypos = Obj->Ypos + Obj->Yspeed;

        // if (Obj->State == IDLE) {
        //         // printf("%d IDLE\n", dt);
        // }

        // if (Obj->State == JUMPING) {
        //         if (Obj->Yspeed < 0) {
        //                 // rising up
        //                 // puts("JUMPING: rising up");
        //                 int amt = abs(Obj->Yspeed);
        //                 new_Ypos -= amt;
        //         } else {
        //                 // falling down
        //                 // puts("JUMPING: falling down");
        //                 int amt = abs(Obj->Yspeed);
        //                 new_Ypos += amt;
        //         }
        // }


        // // COLLISION HANDLING
        // // NOTE: Update positions *HERE* only
        // {
        //         if (TileBelow) {
        //                 const int new_obj_bottom = new_Ypos + Obj->Height;
        //                 const int will_intersect_tile_below =
        //                     (new_obj_bottom > TOP(TileBelow));
        //                 if (will_intersect_tile_below) {
        //                         // stand on the tile.
        //                         // set proper new_Ypos
        //                         new_Ypos    = TOP(TileBelow) - Obj->Height;
        //                         Obj->Yspeed = 0;
        //                         Obj->State  = IDLE;
        //                 }
        //         }
        // }

        // // update positons
        // set_pos(Obj, new_Xpos, new_Ypos);

        // // win/lose condition
        // {
        //         if (BOTTOM(Obj) >= gs->ScreenHeight) {
        //                 puts("AUTO-RESET");
        //                 if (Obj->Type == PLAYER) player_reset(Obj, 200, gs->GroundLevel);
        //         }
        // }
}


void step_tiles(gamestate* gs, unsigned int dt)
{
        // for (int i = 0; i < gs->TileCount; ++i) {
        //         tile* Tile = &gs->Tiles[i];

        //         int new_Xpos = Tile->Xpos + Tile->Xspeed;
        //         int new_Ypos = Tile->Ypos + Tile->Yspeed;

        //         const int new_right_edge  = new_Xpos + Tile->Width;
        //         const int new_left_edge   = new_Xpos;
        //         const int new_top_edge    = new_Ypos;
        //         const int new_bottom_edge = new_Ypos + Tile->Height;

        //         // collision
        //         if (new_right_edge >= gs->ScreenWidth) {
        //                 Tile->Xspeed = -Tile->Xspeed;
        //                 new_Xpos     = new_right_edge - Tile->Width;
        //         }
        //         if (new_left_edge < 0) {
        //                 Tile->Xspeed = -Tile->Xspeed;
        //                 new_Xpos     = 0;
        //         }

        //         // TODO: define magic numbers(30 etc.) SOMEWHERE!
        //         if (new_top_edge < 30) {
        //                 Tile->Yspeed = -Tile->Yspeed;
        //                 new_Ypos     = 30;
        //         }
        //         if (new_bottom_edge >= gs->ScreenHeight - 30) {
        //                 Tile->Yspeed = -Tile->Yspeed;
        //                 new_Ypos     = gs->ScreenHeight - 30 - Tile->Height;
        //         }

        //         // update position
        //         set_pos(Tile, new_Xpos, new_Ypos);
        // }
}
