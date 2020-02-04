#pragma once

#include "object.h"

struct player : public object {
    bool Dead;

public:
    player();
    void update() override;
    void draw() override;

    void lose_life();
};
