#include "object.h"

#include "breakout.h"

object::object()
{
    X = Y = W = H = 0;

    Name = "Object";
    Hit  = 0;
    Vel  = vec2_create(0, 0);

    IsAnimating = 0;
    Anim        = {};

    TexRow = TexCol = 0;
    Hidden = 0;
}

void object::Update() {}

void object::Draw() {}
