#include "particles.h"
#include "game.h"

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

        const float factor = game::is_slow_motion ? game::slow_motion_factor : 1;
        p->X += p->Vel.X * factor;
        p->Y += p->Vel.Y * factor;

        if (p->X > xmax || p->X < xmin || p->Y > ymax || p->Y < ymin) {
            // particle out of bounds. Don't draw it.
            p->Hidden = 1;
            HiddenCount++;
        }


        SDL_Rect tex_rect      = gfx::texture_rect(p->TexRow, p->TexCol);
        SDL_Rect particle_rect = p->abs_rect();
        gfx::draw_texture(gfx::Texture, &tex_rect, &particle_rect);
    }
}

void particle_src::reset_particle(object* p)
{
    const vec2 max_vel{1, 7};


    // random direction
    int _rand_dir = rand() % 2 ? -1 : 1;

    const vec2 max_off{15, 25};

    _rand_dir      = rand() % 2 ? -1 : 1;
    const int xoff = _rand_dir * (rand() % (int) max_off.X);

    _rand_dir      = rand() % 2 ? -1 : 1;
    const int yoff = _rand_dir * (rand() % (int) max_off.Y);

    const int min_width  = TILE_WIDTH / 10;
    const int min_height = TILE_HEIGHT / 10;
    const int max_width  = min_width * 3;
    const int max_height = min_height * 3;

    p->X = X + xoff;
    p->Y = Y + yoff;
    p->W = min_width + (rand() % max_width);
    p->H = min_height + (rand() % max_height);

    const int min_x_vel = 1;
    const int min_y_vel = -2;
    const int max_x_vel = 2;
    const int max_y_vel = -4;

    _rand_dir = rand() % 2 ? -1 : 1;
    int vel_x = min_x_vel + (rand() % max_x_vel);
    vel_x *= _rand_dir;

    int vel_y = min_y_vel - (rand() % max_y_vel);

    p->Vel.X = vel_x;
    p->Vel.Y = vel_y;

    p->Hidden = 0;
}
