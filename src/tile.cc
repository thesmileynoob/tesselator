#include "tile.h"
#include "breakout.h"
#include "game.h"

#include <cassert>

tile::tile()
    : object()  // call parent constructor
{
    // X, Y, TexRow, TexCol must be set manually
    W = TILE_WIDTH;
    H = TILE_HEIGHT;
}

void tile::draw()
{
    object::draw();

    if (Hit || Hidden) return;  // don't draw if Hit or Hidden

    const int pad      = 12;
    SDL_Rect tile_rect = abs_rect();
    tile_rect.x += pad;
    tile_rect.y += pad;
    tile_rect.w -= 2 * pad;
    tile_rect.h -= 2 * pad;

    SDL_Rect tex_rect = gfx::texture_rect(TexRow, TexCol);

    gfx::draw_texture(gfx::Texture, &tex_rect, &tile_rect);
}
