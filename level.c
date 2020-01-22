#include "object.h"

static level LVL_0 = {
    .Rows = 4,
    .Cols = 5,

    .TileWidth  = TILEWIDTH,
    .TileHeight = TILEHEIGHT,
    .Pad        = 18,
    .PlayerLine = 650,

    .Texture = NULL,
    .Tiles =
        (int*){
            0, 0, 0, 0, 0,    //
            0, 0, 0, 0, 0,    //
            0, 0, 0, 0, 0,    //
            0, 0, 0, 0, 0,    //
        },
};

level load_level(int number)
{
        level Level;
        switch (number) {
        case 0:
        default: Level = LVL_0; break;
        }

        if (Level.Texture == NULL) {
                // load texture if not already
                Level.Texture = load_texture("../assets/tiles.png");
        }
        return Level;
}
