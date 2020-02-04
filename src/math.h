#pragma once

struct vec2 {
public:
    float X, Y;
    float _X, _Y;  // original values

public:
    vec2();
    vec2(float x, float y);

    void Scale(float scale);
    void Reset();
};

// float rect
// struct frect {
//     float x, y, w, h;
//     frect();
//     frect(float x, float y, float w, float h);
// };
