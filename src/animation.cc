#include "animation.h"
#include "game.h"

#include <cmath>
#include <cstdio>


void animation::Tick(unsigned int _DT)
{
    assert(!Done);

    const unsigned int max_dt = Time - Elapsed;
    const unsigned int dt     = std::min(_DT, max_dt);  // NEVER OVERSHOOT (in life? :( )
    Elapsed += dt;


    // jump?
    const int ymin = 500;
    const int y    = game::Player->Y;

    // demonstration of another end condition
    if (y <= ymin) {
        Done = true;
        return;
    }

    // puts("tick");
    game::Player->Y -= 10;
    // printf("p->Y: %d\n", game::Player->Y);
}


bool animation::IsDone()
{
    if (Done || Elapsed >= Time) {
        // printf("done\n");
        return true;
    }
}
