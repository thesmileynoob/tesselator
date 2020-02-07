#pragma once

#include "object.h"

/** Particle system
 * - Particles are "spawned" at X, Y
 * - A particle is 'Hidden' when it goes out of bounds
 * - Once all particles are hidden, ie HiddenCount == ParticleCount,
 *   the particle_src is deleted from the vector in game::PSources
 */
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
