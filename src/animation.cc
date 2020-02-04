#include "animation.h"
#include "game.h"

#include <cmath>
#include <cstdio>

const char* _tag_str[] = {
    "ANIM_NO_TAG",
    "ANIM_PLAYER_LOSE_LIFE",
};

animation::animation()
    : animation(ANIM_NO_TAG, 0)
{
}

animation::animation(animation_tag tag, int time_ms)
    : Done{false}
    , Tag{tag}
    , Time{time_ms}
    , Elapsed{0}
    , ShouldRun{false}
{
}

const char* animation::get_name() const { return _tag_str[Tag]; }

bool animation::is_done() const { return Done || (Elapsed >= Time); }

void animation::call_the_right_method(unsigned int dt)
{
    switch (Tag) {
    case ANIM_NO_TAG: assert(0); break;  // should never happen
    case ANIM_PLAYER_LOSE_LIFE: player_lose_life(dt); break;
    default: assert(0); break;
    }
}

void animation::tick(unsigned int _DT)
{
    assert(!Done);

    const unsigned int max_dt = Time - Elapsed;

    int final_dt = std::min(_DT, max_dt);  // NEVER OVERSHOOT (in life? :( )
    Elapsed += final_dt;

    // all the methods are guranteed correct Elapsed, Dt times
    call_the_right_method(final_dt);
}

void animation::player_lose_life(unsigned int dt)
{
    // done condition
    {
        const int y = game::Player->Y;
        if (y >= SCR_HEIGHT + 500) {
            Done = true;
            game::on_game_over();
            return;
        } else if (Elapsed >= Time) {
            Done = true;
            game::on_game_over();
            return;
        }
    }

    // blink every 300 ms
    if (Elapsed < 150) {
        game::Player->Hidden = true;
    } else if (Elapsed < 300) {
        game::Player->Hidden = false;
    } else if (Elapsed < 450) {
        game::Player->Hidden = true;
    } else if (Elapsed < 600) {
        game::Player->Hidden = false;
    } else if (Elapsed < 950) {
        game::Player->Hidden = true;
    }

    // slide off screen to the bottom
    game::Player->Y += 5;
}
