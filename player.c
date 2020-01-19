#include "object.h"

void reset_player(object* Player, int xpos, int ypos)
{
        object result = {0};
        result.Width  = 25;
        result.Height = 50;
        result.Xpos   = xpos;
        result.Ypos   = ypos - result.Height;
        result.State  = IDLE;
        result.Type   = PLAYER;

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
                const tile* Tile = &Tiles[i];
                const int within_xrange =
                    (LEFT(Obj) >= LEFT(Tile) && LEFT(Obj) <= RIGHT(Tile)) ||
                    (RIGHT(Obj) >= LEFT(Tile) && RIGHT(Obj) <= RIGHT(Tile));
                if (!within_xrange) {
                        // TODO: continue loop
                }

                const int obj_above_tile = (BOTTOM(Obj) <= TOP(Tile));
                if (!obj_above_tile) {
                        // TODO: continue loop
                }

                // update Result
                const int old_top = Result == -1 ? TOP(Tile) : Tiles[Result].Ypos;
                const int this_above_old_tile = (TOP(Tile) <= old_top);
                if (this_above_old_tile && within_xrange && obj_above_tile) {
                        // update Result
                        Result = i;
                }
        }

        return Result;
}
