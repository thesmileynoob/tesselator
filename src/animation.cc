#include "animation.h"
#include "game.h"

#include <cmath>
#include <cstdio>

const char* _tag_str[] = {
    "ANIM_NO_TAG",
    "ANIM_PLAYER_LOSE_LIFE",
    "ANIM_GAME_OVER_DELAY",
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

void animation::mark_done()
{
    printf("animation done: %s\n", get_name());
    Done = true;
    return;
}

void animation::tick(unsigned int _DT)
{
    assert(!Done);  // avoid ticking once animation is finished

    const unsigned int max_dt = Time - Elapsed;

    int final_dt = std::min(_DT, max_dt);  // NEVER OVERSHOOT (in life? :( )
    Elapsed += final_dt;

    // all the methods are guranteed correct Elapsed, Dt times
    call_the_right_method(final_dt);
}


void animation::call_the_right_method(unsigned int dt)
{
    switch (Tag) {
    case ANIM_NO_TAG: assert(0); break;  // should never happen
    case ANIM_PLAYER_LOSE_LIFE: player_lose_life(dt); break;
    case ANIM_GAME_OVER_DELAY: game_over_delay(dt); break;
    default: assert(0); break;
    }
}


void animation::player_lose_life(unsigned int dt)
{
    // done conditions
    {
        // if any of the following conditions is true, stop!
        const int y = game::Player->Y;
        if (y >= SCR_HEIGHT + 500) {
            mark_done();
            game::on_game_over(game::GAME_OVER_DEAD);
            return;
        } else if (Elapsed >= Time) {
            mark_done();
            game::on_game_over(game::GAME_OVER_DEAD);
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

    game::slow_motion_factor = .7;
    game::is_slow_motion     = true;
}

void animation::game_over_delay(unsigned int dt)
{
    // done condition
    {
        if (Elapsed >= Time) {
            mark_done();
            game::Running = false;
            return;
        }
    }

    // ultimate objective
    // GTA:V (tm)
    game::slow_motion_factor = .3;
    game::is_slow_motion     = true;
}
