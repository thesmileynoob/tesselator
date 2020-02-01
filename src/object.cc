#include "object.h"

#include "breakout.h"

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

bool object::IsInside(float left, float right, float top, float bottom)
{
    const int within_x_range = LEFT(this) >= left && RIGHT(this) <= right;
    const int within_y_range = TOP(this) >= top && BOTTOM(this) <= bottom;
    return within_x_range && within_y_range;
}

namespace game
{

// todo move to main.cc
int level_left   = FRAME_WIDTH;
int level_right  = SCR_WIDTH - FRAME_WIDTH;
int level_top    = FRAME_WIDTH;
int level_bottom = SCR_HEIGHT - FRAME_WIDTH;

}  // namespace game
