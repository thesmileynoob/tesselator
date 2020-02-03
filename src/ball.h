#pragma once

#include "object.h"

struct ball : public object {

public:
    ball();
    void Update() override;
    void Draw() override;

private:
    void handleCollision(object* obj);
};
