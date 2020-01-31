#pragma once

struct object;
struct animation {
    int (*anim_func)(struct object*);
    int frame_count;
};
