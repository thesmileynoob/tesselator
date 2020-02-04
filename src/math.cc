#include "math.h"

vec2::vec2()
    : vec2(0, 0)
{
}

vec2::vec2(float x, float y)
    : X(x)
    , Y(y)
    , _X(x)
    , _Y(y)
{
}


void vec2::scale(float scale)
{
    X *= scale;
    Y *= scale;
}


void vec2::reset()
{
    X = _X;
    Y = _Y;
}
