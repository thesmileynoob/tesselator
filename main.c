#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "object.h"


const int ScreenWidth  = 1280;
const int ScreenHeight = 720;

const int GroundLevel        = (int) ScreenHeight * 2 / 4;
static int visual_debug      = 1;
static int highlight_tile_id = -1;    // -1 == no highlight

SDL_Window* window     = NULL;
SDL_Renderer* renderer = NULL;


tile Tiles[] = {
    // X, Y, W, H, r,g,b
    {100, GroundLevel, 200, 10, /*rgb*/ 0, 0, 200},
    {300, GroundLevel + 200, 200, 10, /*rgb*/ 0, 0, 200},
    {200, GroundLevel + 300, 500, 10, /*rgb*/ 0, 0, 200},
    //     {500, GroundLevel - 200, 100, 200, /*rgb*/ 150, 50, 0},
    //     {300, GroundLevel - 100, 100, 100, /*rgb*/ 150, 50, 0},
};


static int init_sdl();
static int deinit_sdl();
static void handle_input(const Uint8* keys, object* player, unsigned int dt);
static unsigned int get_dt();

inline static int tile_count() { return sizeof(Tiles) / sizeof(tile); }
inline static SDL_Rect tile_rect(const tile* Tile)
{
        return (SDL_Rect){Tile->Xpos, Tile->Ypos, Tile->Width, Tile->Height};
}


int main(int argc, char const* argv[])
{
        init_sdl();
        assert(window && renderer);

        // player init
        object Player;
        reset_player(&Player, 200, GroundLevel);


        int running = 1;
        while (running) {
                SDL_Event ev;
                while (SDL_PollEvent(&ev)) {
                        SDL_Keycode sym = ev.key.keysym.sym;
                        if (sym == SDLK_q || ev.type == SDL_QUIT) { running = 0; }

                        if (ev.type == SDL_KEYUP) {
                                // on key release
                                if (sym == SDLK_v) { visual_debug = !visual_debug; }
                        } else if (ev.type == SDL_KEYDOWN) {
                                // on key press
                                // RESET
                                if (sym == SDLK_r) {
                                        puts("MANUAL RESET");
                                        reset_player(&Player, 200, GroundLevel);
                                }
                        }
                }

                unsigned int dt = get_dt();

                // input
                handle_input(SDL_GetKeyboardState(NULL), &Player, dt);

                // START DRAWING
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderClear(renderer);

                // level
                {
                        const int tc = tile_count();
                        for (int i = 0; i < tc; ++i) {
                                tile* Tile = &Tiles[i];
                                // SDL_Rect TileRect = {Tile->Xpos, Tile->Ypos,
                                // Tile->Width,
                                //                      Tile->Height};
                                SDL_Rect TileRect = tile_rect(Tile);
                                SDL_SetRenderDrawColor(renderer, Tile->r, Tile->g,
                                                       Tile->b, 255);

                                if (highlight_tile_id == i) {
                                        // paint it with player color
                                        SDL_SetRenderDrawColor(renderer, Player.r,
                                                               Player.g, Player.b, 255);
                                }
                                SDL_RenderFillRect(renderer, &TileRect);
                                SDL_RenderDrawRect(renderer, &TileRect);
                        }
                }


                // objects
                {
                        // player
                        SDL_SetRenderDrawColor(renderer, Player.r, Player.g, Player.b,
                                               255);
                        SDL_Rect PlayerRect = {Player.Xpos, Player.Ypos, Player.Width,
                                               Player.Height};
                        SDL_RenderFillRect(renderer, &PlayerRect);
                        SDL_RenderDrawRect(renderer, &PlayerRect);
                }

                // visual debug
                if (visual_debug) {
                        // redraw player in different color
                        // printf("VDB: %d\n", visual_debug);
                        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
                        SDL_Rect PlayerRect = {Player.Xpos, Player.Ypos, Player.Width,
                                               Player.Height};
                        SDL_RenderFillRect(renderer, &PlayerRect);
                        SDL_RenderDrawRect(renderer, &PlayerRect);

                        // draw velocity vector
                        const int scale = 2;

                        const int x1 = Player.Xpos + (Player.Width / 2);
                        const int y1 = Player.Ypos + (Player.Height / 2);
                        const int x2 = x1 + Player.Xspeed * scale;
                        const int y2 = y1 + Player.Yspeed * scale;


                        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
                        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                }


                SDL_RenderPresent(renderer);
                // END DRAWING

                SDL_Delay(1000 / 60);
        }


        return 0;
}


tile* get_tile_below_object(const object* Obj)
{
        int id = tile_below_object(Obj, Tiles, tile_count());
        if (id == -1)
                return NULL;
        else
                return &Tiles[id];
}

void step(object* Obj, unsigned int dt)
{
        // check if there's a tile below object
        highlight_tile_id = tile_below_object(Obj, Tiles, tile_count());
        printf("id: %d\n", highlight_tile_id);

        // TODO: get_tile_{left, right, top}
        // Collect data required for the step
        const tile* TileBelow = get_tile_below_object(Obj);    // NULLABLE
        const int gravity     = 2;

        // new values to be assigned to the boject
        int new_Xpos, new_Ypos;
        new_Xpos = Obj->Xpos + Obj->Xspeed;

        // apply gravity
        Obj->Yspeed += gravity;
        new_Ypos = Obj->Ypos + Obj->Yspeed;

        if (Obj->State == IDLE) {
                // printf("%d IDLE\n", dt);
        }

        if (Obj->State == JUMPING) {
                if (Obj->Yspeed < 0) {
                        // rising up
                        // puts("JUMPING: rising up");
                        int amt = abs(Obj->Yspeed);
                        new_Ypos -= amt;
                } else {
                        // falling down
                        // puts("JUMPING: falling down");
                        int amt = abs(Obj->Yspeed);
                        new_Ypos += amt;
                }
        }


        // COLLISION HANDLING
        // NOTE: Update positions *HERE* only
        {
                if (TileBelow) {
                        const int new_obj_bottom            = new_Ypos + Obj->Height;
                        const int tile_top                  = TileBelow->Ypos;
                        const int will_intersect_tile_below = (new_obj_bottom > tile_top);
                        if (will_intersect_tile_below) {
                                // stand on the tile.
                                // set proper new_Ypos
                                new_Ypos    = tile_top - Obj->Height;
                                Obj->Yspeed = 0;
                                Obj->State  = IDLE;
                        }
                }

                // update x postion freely
                Obj->Xpos = new_Xpos;
                Obj->Ypos = new_Ypos;
        }


        // win/lose condition
        {
                const int obj_bottom = Obj->Ypos + Obj->Height;
                if (obj_bottom >= ScreenHeight) {
                        puts("AUTO-RESET");
                        reset_player(Obj, 200, GroundLevel);
                }
        }
}

static void handle_input(const Uint8* Keys, object* Player, unsigned int dt)
{
        // NOTE:
        //      DO *NOT* UPDATE POSITIONS (*pos) IN THIS FUNCITON!
        //      *ONLY* UPDATE STATES and SPEEDS!

        // lateral movement
        if (Keys[SDL_SCANCODE_A]) {
                Player->Xspeed = -15;    // go left
        } else if (Keys[SDL_SCANCODE_D]) {
                Player->Xspeed = +15;    // go right
        } else {
                Player->Xspeed = 0;
        }

        // vertical movement
        if (Keys[SDL_SCANCODE_SPACE] && Player->State == IDLE) {
                Player->State  = JUMPING;
                Player->Yspeed = -Player->JumpSpeed;
        }

        step(Player, dt);
}

int init_sdl()
{
        int err = 0;
        err     = SDL_Init(SDL_INIT_EVERYTHING);
        if (err) {
                printf("SDL2 Error\n");
                exit(1);
        }
        err =
            SDL_CreateWindowAndRenderer(ScreenWidth, ScreenHeight, 0, &window, &renderer);
        if (err) {
                printf("SDL2 Window Error\n");
                deinit_sdl();
        }

        return 0;
}

int deinit_sdl()
{
        SDL_Quit();
        return 0;
}

/**
 * returns: dt in millisecs
 */
unsigned int get_dt()
{
        static unsigned int oldtime = 0;
        const unsigned int newtime  = SDL_GetTicks();
        const unsigned int dt       = newtime - oldtime;
        oldtime                     = newtime;
        return dt;
}
