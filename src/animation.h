#pragma once


enum animation_tag {
    ANIM_PLAYER_LOSE_LIFE,  // Player lost a life
};

struct animation {
public:
    animation_tag Tag;
    int Time;
    int Elapsed;
private:
    bool Done;

public:
    void Tick(unsigned int DT);
    bool IsDone();
};
