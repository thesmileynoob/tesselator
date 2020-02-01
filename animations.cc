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
    // printf("animating: %s\n", __func__);
    const int gravity = 3;
    static int yspeed = 0;

    if (t->Anim.frame_count <= 0) {
        assert(t->IsAnimating == 1);
        t->IsAnimating = 0;
        assert(t->IsAnimating == 0);
        yspeed = 0;
        return 0;
    }
    yspeed -= gravity;

    t->Y -= yspeed;
    return 1;
}
