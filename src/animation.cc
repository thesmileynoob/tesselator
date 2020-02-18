#include "animation.h"
#include "game.h"
#include "breakout.h"

#include <cmath>
#include <cstdio>

const char* _tag_str[] = {
    "ANIM_NO_TAG",            //
    "ANIM_PLAYER_LOSE_LIFE",  //
    "ANIM_GAME_OVER_DELAY",   //
    "ANIM_BLINK_SCREEN",      //
    "ANIM_PLAYER_HIT",        //
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
    // printf("animation done: %s\n", get_name());
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
    case ANIM_BLINK_SCREEN: blink_screen(dt); break;
    case ANIM_PLAYER_HIT: player_hit(dt); break;
    default: assert(0); break;
    }
}


void animation::player_lose_life(unsigned int dt)
{
    // done conditions
    {
        // if any of the following conditions is true, stop!
        const int y = game::Player->Y;
        if (y >= gfx::SCR_HEIGHT + 500) {
            mark_done();
            game::event_game_over(game::GAME_OVER_DEAD);
            return;
        } else if (Elapsed >= Time) {
            mark_done();
            game::event_game_over(game::GAME_OVER_DEAD);
            return;
        }
    }

    // blink every blink_freq ms
    const int blink_freq    = 100;             // ms
    const int blink_freq_x2 = 2 * blink_freq;  // ms

    // remainder goes from 0 to twice the blink_freq (blink_freq_x2).
    // because of this, we can turn on and off (ie "blink") the player
    // half of the time (ie once every half blink_freq_x2 ms).
    /// NOTE: I don't know how I came up with this. Human brain > worm hole?
    const int remainder = Elapsed % blink_freq_x2;  // [0, 2 * blink_freq]
    if (remainder < blink_freq) {
        game::Player->Hidden = true;
    } else {
        game::Player->Hidden = false;
    }

    /** previous version of blink for historical reasons. here freq is 150 ms */
    // if (Elapsed < 150) {
    //     game::Player->Hidden = true;
    // } else if (Elapsed < 300) {
    //     game::Player->Hidden = false;
    // } else if (Elapsed < 450) {
    //     game::Player->Hidden = true;
    // } else if (Elapsed < 600) {
    //     game::Player->Hidden = false;
    // } else if (Elapsed < 950) {
    //     game::Player->Hidden = true;
    // }

    // slide off the bottom edge of the screen
    game::Player->Y += 3;

    game::slow_motion_factor = .5;
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
    game::slow_motion_factor = .2;
    game::is_slow_motion     = true;
}


void animation::blink_screen(unsigned int dt)
{
    // done condition
    {
        if (Elapsed >= Time) {
            mark_done();
            return;
        }
    }

    // change color for 'Time' ms and back
    const int blink_freq    = Time / 2;        // ms
    const int blink_freq_x2 = 2 * blink_freq;  // ms

    const int remainder = Elapsed % blink_freq_x2;  // [0, 2 * blink_freq]

    if (remainder < blink_freq) {
        game::BgCol = {2, 23, 90, 255};
    } else {
        game::BgCol = {0, 0, 0, 255};
    }
}


void animation::player_hit(unsigned int dt)
{
    // done condition
    {
        if (Elapsed >= Time) {
            mark_done();
            return;
        }
    }

    // change color for 'Time' ms and back
    const int blink_freq    = Time / 2;        // ms
    const int blink_freq_x2 = 2 * blink_freq;  // ms

    const int remainder = Elapsed % blink_freq_x2;  // [0, 2 * blink_freq]

    const float rate_x = .8;  // px
    const float rate_y = .5;  // px

    if (remainder < blink_freq) {
        game::Player->W += 2 * rate_x;
        game::Player->X -= rate_x;

        game::Player->H += 2 * rate_y;
        game::Player->Y -= rate_y;
    } else {
        game::Player->W -= 2 * rate_x;
        game::Player->X += rate_x;

        game::Player->H -= 2 * rate_y;
        game::Player->Y += rate_y;
    }
}
