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

void object::Update() {}

void object::Draw() {}

bool object::IsInside() { return IsInside(0, game::level_width, 0, game::level_height); }

bool object::IsInside(float left, float right, float top, float bottom)
{
    const int within_x_range = LEFT(this) >= left && RIGHT(this) <= right;
    const int within_y_range = TOP(this) >= top && BOTTOM(this) <= bottom;
    return within_x_range && within_y_range;
}


// relative to level
int object::Left() const { return X; }
int object::Right() const { return Left() + W; }
int object::Top() const { return Y; }
int object::Bottom() const { return Top() + H; }
SDL_Rect object::Rect() const { return {Left(), Top(), W, H}; };
vec2 object::Center() const { return {Left() + (W / 2), Top() + (H / 2)}; }

// absolute
int object::AbsLeft() const { return game::level_left + X; }
int object::AbsRight() const { return Left() + W; }
int object::AbsTop() const { return game::level_top + Y; }
int object::AbsBottom() const { return Top() + H; }
SDL_Rect object::AbsRect() const { return {AbsLeft(), AbsTop(), W, H}; };
vec2 object::AbsCenter() const { return {AbsLeft() + (W / 2), AbsTop() + (H / 2)}; }
