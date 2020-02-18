#include "level.h"

void level::update()
{
    Player->update();
    Ball->update();
}


void level::draw()
{
    // TILES
    {
        SDL_SetRenderDrawColor(gfx::_renderer, 255, 0, 0, 255);
        for (int i = 0; i < TileCount; ++i) {
            tile* t = &Tiles[i];
            t->draw();
        }
    }

    // PLAYER
    {
        Player->draw();
    }

    // BALL
    {
        Ball->draw();
    }
}
