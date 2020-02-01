#include "tile.h"
#include "breakout.h"

#include <cassert>

tile::tile()
    : object()  // call parent constructor
{

    // X, Y must be set manually
    // TexRow, TexCol must be set manually

    W = TILE_WIDTH;
    H = TILE_HEIGHT;
}

void tile::Draw()
{
    object::Draw();
    if (Hit || Hidden) return;  // don't draw if Hit
    const int pad     = 12;
    const int x       = pad + X;
    const int y       = pad + Y;
    const int w       = W;
    const int h       = H;
    SDL_Rect tex_rect = gfx::texture_rect(TexRow, TexCol);

    SDL_Rect tile_rect = {x, y, w - 2 * pad, h - 2 * pad};
    // SDL_RenderCopy(gfx::_renderer, Texture, &tex_rect, &tile_rect);
    gfx::draw_texture(gfx::Texture, &tex_rect, &tile_rect);
}
