#pragma once


enum animation_tag {
    ANIM_NO_TAG = 0,        // should never happen
    ANIM_PLAYER_LOSE_LIFE,  // Player lost a life
};

struct animation {

private:
    bool Done;
    int Dt;

public:
    animation_tag Tag;
    int Time;
    int Elapsed;
    bool ShouldRun;

public:
    animation();
    animation(animation_tag tag, int time_ms);

    bool IsDone() const;
    void Tick(unsigned int DT);


private:
    void call_the_right_method();
    void player_lose_life();
};
