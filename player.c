#include "object.h"

void player_reset(object* Player, int xpos, int ypos)
{
        object result = *Player;
        result.Width  = 25;
        result.Height = 50;
        result.Xpos   = xpos;
        result.Ypos   = ypos - result.Height;
        result.State  = IDLE;
        result.Type   = PLAYER;

        result.Xspeed    = 10;
        result.Yspeed    = 0;
        result.JumpSpeed = .4 * result.Height;
        result.r         = 0;
        result.g         = 0;
        result.b         = 0;

        result.FaceRight = 1;

        *Player = result;
}
