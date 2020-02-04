#pragma once

#include "object.h"

struct player : public object {

public:
    player();
    void update() override;
    void Draw() override;

    void LoseLife();
};
