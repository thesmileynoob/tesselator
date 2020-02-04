#pragma once

#include "object.h"

struct player : public object {

public:
    player();
    void update() override;
    void draw() override;

    void lose_life();
};
