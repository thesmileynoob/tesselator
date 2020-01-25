#include "breakout.h"
#include <assert.h>


/** Animation end procedure */
#define CHECK_ANIM_END(Obj)           \
    if (Obj->Anim.frame_count <= 0) { \
        assert(t->IsAnimating == 1);  \
        Obj->IsAnimating = 0;         \
        goto on_anim_end;             \
    }

/** when a tile is hit
 * NOTE: This has to be sweeeeeet!
 * RETURNS: 0 on animation end
 */
int anim_tile_breakout_animation(tile* t)
{
    CHECK_ANIM_END(t)

    // printf("animating: %s\n", __func__);
    const int gravity = 3;
    static int yspeed = 0;
    yspeed -= gravity;

    t->Y -= yspeed;
    return 1;

on_anim_end:
    assert(t->IsAnimating == 0);
    yspeed = 0;
    return 0;
}

int anim_player_expand(tile* t)
{
    CHECK_ANIM_END(t)

    // printf("animating: %s\n", __func__);
    const int amt = 5;
    t->X -= amt;
    t->W += 2 * amt;
    return 1;

on_anim_end:
    assert(t->IsAnimating == 0);
    return 0;
}
