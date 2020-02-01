#pragma once

#include "object.h"

struct player : public object {

public:
    player();
    void Update() override;
    void Draw() override;
    int Animate() override;
};
