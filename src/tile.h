#pragma once

#include "object.h"

struct tile : public object {

public:
    tile();
    void draw() override;
};
