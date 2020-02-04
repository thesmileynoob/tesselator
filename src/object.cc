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

void object::Draw()
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

bool object::IsInside(float left, float right, float top, float bottom)
{
    const bool within_x_range = Left() >= left && Right() <= right;
    const bool within_y_range = Top() >= top && Bottom() <= bottom;
    return within_x_range && within_y_range;
}


// relative to level
float object::Left() const { return X; }
float object::Right() const { return Left() + W; }
float object::Top() const { return Y; }
float object::Bottom() const { return Top() + H; }
SDL_Rect object::Rect() const { return {(int) Left(), (int) Top(), (int) W, (int) H}; };
vec2 object::Center() const { return {Left() + (W / 2), Top() + (H / 2)}; }

// absolute
float object::AbsLeft() const { return game::level_left + X; }
float object::AbsRight() const { return Left() + W; }
float object::AbsTop() const { return game::level_top + Y; }
float object::AbsBottom() const { return Top() + H; }
SDL_Rect object::AbsRect() const
{
    return {(int) AbsLeft(), (int) AbsTop(), (int) W, (int) H};
};
vec2 object::AbsCenter() const { return {AbsLeft() + (W / 2), AbsTop() + (H / 2)}; }
