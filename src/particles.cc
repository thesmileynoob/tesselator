#include "particles.h"

particle_src::particle_src(int x, int y, int count, int texrow, int texcol)
    : X(x)
    , Y(y)
    , ParticleCount(count)
    , Particles(nullptr)
    , HiddenCount(0)
{
    // todo new
    Particles = (object*) calloc(ParticleCount, sizeof(object));
    assert(Particles);

    for (int i = 0; i < ParticleCount; ++i) {
        object* p = &Particles[i];
        reset_particle(p);
        p->Hidden = 0;  // initially hidden
        p->TexRow = texrow;
        p->TexCol = texcol;
    }
}

bool particle_src::IsDone() const { return ParticleCount == HiddenCount; }

void particle_src::Draw()
{
    const int radius = 50;

    const int xmin = X - radius;
    const int xmax = X + radius;

    const int ymin = Y - radius;
    const int ymax = Y + radius;

    for (int i = 0; i < ParticleCount; ++i) {
        object* p = &Particles[i];
        if (p->Hidden) continue;

        p->X += p->Vel.X;
        p->Y += p->Vel.Y;

        if (p->X > xmax || p->X < xmin || p->Y > ymax || p->Y < ymin) {
            // particle out of bounds. Don't draw it.
            p->Hidden = 1;
            HiddenCount++;
        }


        SDL_Rect tex_rect      = gfx::texture_rect(p->TexRow, p->TexCol);
        SDL_Rect particle_rect = RECT(p);
        gfx::draw_texture(gfx::Texture, &tex_rect, &particle_rect);
    }
}

void particle_src::reset_particle(object* p)
{
    const int max_vy = 3;

    const int _rand_dir = rand() % 2 ? -1 : 1;  // random direction

    const int xoff = _rand_dir * (rand() % 13);
    const int yoff = _rand_dir * (rand() % 25);

    p->X = X + xoff;
    p->Y = Y + yoff;
    p->W = (5) + rand() % 20;
    p->H = (5) + rand() % 20;


    p->Vel = (vec2){rand() % 2 ? -1 : 1, -1 * rand() % max_vy};


    p->TexRow = rand() % 5;
    p->TexCol = rand() % 5;

    p->Hidden = 0;
}
