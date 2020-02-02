#pragma once

#include "object.h"

struct particle_src {
public:
    int X, Y;  // location
    int ParticleCount;
    object* Particles;
    int HiddenCount;

public:
    particle_src(int x, int y, int count, int texrow, int texcol);
    void Update();
    void Draw();
    bool IsDone() const;

private:
    void reset_particle(object* p);
};
