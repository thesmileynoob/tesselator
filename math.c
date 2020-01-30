#include "breakout.h"

vec2 vec2_create(float x, float y)
{
    vec2 result = {x, y, x, y};
    return result;
}

void vec2_reset(vec2* v)
{
    v->X = v->_X;
    v->Y = v->_Y;
}
