#pragma once

#include "ball.h"
#include "object.h"
#include "player.h"

struct tile : public object {

public:
    tile();
    void draw() override;
};
