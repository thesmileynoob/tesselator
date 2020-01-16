#include "object.h"

void reset_player(object* Player, int xpos, int ypos)
{
        object result = {0};
        result.Width  = 25;
        result.Height = 50;
        result.Xpos   = xpos;
        result.Ypos   = ypos - result.Height;
        result.State  = IDLE;

        result.Xspeed    = 15;
        result.Yspeed    = 0;
        result.JumpSpeed = .5 * result.Height;
        result.r         = 255;

        *Player = result;
}

int tile_below_object(const object* Obj, const tile* Tiles, int TileCount)
{
        int Result = -1;    // index of the tile. -1 -> Tile not found

        for (int i = 0; i < TileCount; ++i) {
                const tile* Tile     = &Tiles[i];
                const int obj_top    = Obj->Ypos;
                const int obj_bottom = Obj->Ypos + Obj->Height;
                const int obj_left   = Obj->Xpos;
                const int obj_right  = Obj->Xpos + Obj->Width;

                const int tile_top    = Tile->Ypos;
                const int tile_bottom = Tile->Ypos + Tile->Height;
                const int tile_left   = Tile->Xpos;
                const int tile_right  = Tile->Xpos + Tile->Width;

                const int within_xrange =
                    (obj_left >= tile_left && obj_left <= tile_right) ||
                    (obj_right >= tile_left && obj_right <= tile_right);
                if (!within_xrange) {
                        // TODO: continue loop
                }

                const int obj_above_tile = (obj_bottom <= tile_top);
                if (!obj_above_tile) {
                        // TODO: continue loop
                }

                // update Result
                const int old_top = Result == -1 ? tile_top : Tiles[Result].Ypos;
                const int this_above_old_tile = (tile_top <= old_top);
                if (this_above_old_tile && within_xrange && obj_above_tile) {
                        // update Result
                        Result = i;
                }
        }

        return Result;
}
