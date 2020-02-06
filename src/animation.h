#pragma once


enum animation_tag {
    ANIM_NO_TAG = 0,        // should never happen
    ANIM_PLAYER_LOSE_LIFE,  // Player lost a life
    ANIM_GAME_OVER_DELAY,   // game over, run a bit more
    ANIM_BLINK_SCREEN,      // blink screen on tile hit
    ANIM_PLAYER_HIT,        // ball hit player
};


struct animation {

private:
    bool Done;  // set to true if animation has "finished"

public:
    animation_tag Tag;
    int Time;
    int Elapsed;
    bool ShouldRun;

public:
    animation();
    animation(animation_tag tag, int time_ms);

    // return true if animation is done. useful to remove animation from the list
    bool is_done() const;

    void tick(unsigned int DT);


    const char* get_name() const;


private:
    // mark the animation as finished. is_done() will return true after this
    void mark_done();

    // call the correct animation method based on 'Tag'
    void call_the_right_method(unsigned int dt);

    ///// methods ////

    void player_lose_life(unsigned int dt);
    // run the game a little more before closing window
    void game_over_delay(unsigned int dt);
    // blinks screen on tile hit
    void blink_screen(unsigned int dt);
    // TODO: documentation
    void player_hit(unsigned int dt);
};
