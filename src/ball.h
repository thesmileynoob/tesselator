#pragma once

#include "object.h"

struct ball : public object {

public:
    ball();
    void update() override;
    void draw() override;

private:
    void handle_collision(object* obj);
};
