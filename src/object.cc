#include "object.h"

#include "breakout.h"
#include "game.h"

object::object()
{
    X = Y = W = H = 0;

    Name = "Object";
    Hit  = 0;
    Vel  = vec2();


    TexRow = TexCol = 0;
    Hidden          = 0;
}

void object::update() {}

void object::draw()
{
    if (Hidden) {
        // puts("warning: drawing hidden obj!\n");
        return;
    }
}

bool object::IsWithinWorld()
{
    return IsInside(0, game::level_width, 0, game::level_height);
}

bool object::IsInside(float _left, float _right, float _top, float _bottom)
{
    const bool within_x_range = left() >= _left && right() <= _right;
    const bool within_y_range = top() >= _top && bottom() <= _bottom;
    return within_x_range && within_y_range;
}


// relative to level
float object::left() const { return X; }
float object::right() const { return left() + W; }
float object::top() const { return Y; }
float object::bottom() const { return top() + H; }
SDL_Rect object::rect() const { return {(int) left(), (int) top(), (int) W, (int) H}; };
vec2 object::center() const { return {left() + (W / 2), top() + (H / 2)}; }

// absolute
float object::abs_left() const { return game::level_left + X; }
float object::abs_right() const { return left() + W; }
float object::abs_top() const { return game::level_top + Y; }
float object::abs_bottom() const { return top() + H; }
SDL_Rect object::abs_rect() const
{
    return {(int) abs_left(), (int) abs_top(), (int) W, (int) H};
};
vec2 object::abs_center() const { return {abs_left() + (W / 2), abs_top() + (H / 2)}; }
