#pragma once

#include "object.h"

struct ball : public object {

public:
    ball();
    void update() override;
    void draw() override;

private:
    void handleCollision(object* obj);
};
